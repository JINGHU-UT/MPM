
#include "phaseStepData.h"


//#define FULL_FEM  // Ignore particles, just solve FEM system

namespace mpm {

void PhaseStepData::computeProjectors(const bool weakStressBC, const std::vector<RigidBodyData> &rbData,
									  Projectors& mats) const
{
	const ID m  = nNodes() ;
	const ID mc = nSuppNodes() ;

	mats.vel.setRows( m );
	mats.vel.reserve( m );

	mats.stress.setRows( m+mc );
	mats.stress.reserve( m+mc );

	for( ID i = 0 ; i < nodes.count() ; ++i  ) {
		const ID idx = nodes.revIndices[ i ] ;

		m_surfaceNodes[idx].velProj( mats.vel.insertBack( i,i ) ) ;

		if( weakStressBC )
			mats.stress.insertBack( i,i ).setIdentity() ;
		else
			m_surfaceNodes[idx].stressProj( mats.stress.insertBack( i,i ) ) ;
	}

	// Additional nodes for frictional boundaries
	for( unsigned k = 0 ; k < rbData.size() ; ++k ) {
		for( ID i = 0 ; i < rbData[k].nodes.count() ; ++i  ) {
			const ID   j = rbData[k].nodes.offset + i ;
			const ID idx = rbData[k].nodes.revIndices[ i ] ;

			// Ignore RB-boundary constraints on Dirichlet boundaries
			if( m_surfaceNodes[idx].bc == BoundaryInfo::Stick )
				mats.stress.insertBack( j,j ).setZero() ;
			else
				mats.stress.insertBack( j,j ).setIdentity() ;
		}
	}

	mats.stress.finalize();
	mats.vel   .finalize();
}

void PhaseStepData::assembleMatrices(
		const Particles &particles,
		const Config &config, const Scalar dt, const MeshType &mesh, const ScalarField &phiInt,
		std::vector< RigidBodyData >&rbData
		)
{
	typedef typename MeshType::Location Loc ;
	typedef typename MeshType::Interpolation Itp ;
	typedef typename MeshType::Derivatives Dcdx ;

	bogus::Timer timer;

	const ID m  = nNodes() ;
	const ID mc = nSuppNodes() ;

	const Scalar mass_regul = 1.e-8 ;

	computeProjectors( config.weakStressBC, rbData, proj ) ;

	// Lumped mass matrix
	{
		forms.M_lumped.setRows( m );
		forms.M_lumped.setIdentity() ;
		forms.M_lumped_inv.setRows( m );
		forms.M_lumped_inv.setIdentity() ;
		forms.M_lumped_inv_sqrt.setRows( m );
		forms.M_lumped_inv_sqrt.setIdentity() ;

	#pragma omp parallel for
		for( ID i = 0 ; i < m ; ++i ) {
			forms.M_lumped.block( i ) *= phiInt[ nodes.revIndices[i] ] ;
		}
	}

	// Other bilinear forms
	{
		timer.reset() ;

		FormBuilder builder( mesh ) ;
		builder.reset( m );
		builder.addToID( nodes.cells, nodes.indices, nodes.indices );
		builder.makeCompressed();

		Log::Debug() << "ID computation: " << timer.elapsed() << std::endl ;

		// A
		forms.A.clear();
		forms.A.setRows( m );
		forms.A.setCols( m );
		forms.A.cloneIndex( builder.index() ) ;
		forms.A.setBlocksToZero() ;
		// J
		forms.J.clear();
		forms.J.setRows( m );
		forms.J.setCols( m );
		forms.J.cloneIndex( builder.index() ) ;
		forms.J.setBlocksToZero() ;

		// C
		if( config.enforceMaxFrac ) {
			forms.C.clear();
			forms.C.setRows( builder.rows() );
			forms.C.setCols( m );
			forms.C.cloneIndex( builder.index() ) ;
			forms.C.setBlocksToZero() ;
		}

		builder.addRows(mc) ;

		// B
		forms.B.clear();
		forms.B.setRows( builder.rows() );
		forms.B.setCols( m );
		forms.B.cloneIndex( builder.index() ) ;
		forms.B.setBlocksToZero() ;

		timer.reset() ;
#ifdef FULL_FEM
		builder.integrate_qp( nodes.cells, [&]( Scalar w, const Loc&, const Itp& itp, const Dcdx& dc_dx )
			{
				FormBuilder:: addDuDv( forms.A, w, itp, dc_dx, nodes.indices, nodes.indices ) ;
			}
		);
		Log::Debug() << "Integrate grid: " << timer.elapsed() << std::endl ;
#endif

		timer.reset() ;

#ifndef INTEGRATE_PARTICLES_SEQUENTIAL
		{
			// Integrating over particles can be slow and not directly parallelizable
			// To regain some parallelism, we first associate particles to nodes,
			// then compute separately each row of he form matrices

			const size_t n = particles.count() ;

			Eigen::Matrix< Scalar, MeshType::NV, Eigen::Dynamic > coeffs ;
			Eigen::Matrix<  ID, MeshType::NV, Eigen::Dynamic > nodeIds  ;
			coeffs  .resize( MeshType::NV, n) ;
			nodeIds.resize( MeshType::NV, n) ;

			std::vector< std::vector< std::pair< size_t, ID > > > nodeParticles ( m ) ;


			Loc loc ;
			Itp itp ;
			Dcdx dc_dx ;

#pragma omp parallel private( loc, itp )
			for ( size_t i = 0 ; i < n ; ++i )
			{
				mesh.locate( particles.centers().col(i), loc );
				mesh.interpolate( loc, itp );
				nodeIds.col(i) = itp.nodes ;
				coeffs .col(i) = itp.coeffs ;
			}

			for ( size_t i = 0 ; i < n ; ++i ) {
				for( ID k = 0 ; k < MeshType::NV ; ++k ) {
					nodeParticles[ nodes.indices[nodeIds(k,i)] ].push_back( std::make_pair(i,k) ) ;
				}
			}

#pragma omp parallel for private( loc, itp, dc_dx )
			for( ID nidx = 0 ; nidx < m ; ++ nidx )
			{
				for( unsigned i = 0 ; i < nodeParticles[nidx].size() ; ++i ) {
					const size_t pid = nodeParticles[nidx][i].first ;
					const ID k0   = nodeParticles[nidx][i].second ;
					mesh.locate( particles.centers().col(pid), loc );
					mesh.get_derivatives( loc, dc_dx );
					itp.nodes  = nodeIds.col(pid) ;
					itp.coeffs = coeffs.col(pid) ;

					const Scalar w = particles.volumes()[pid] ;
					const Scalar m = itp.coeffs[k0] * w ;
					const Dcdx& const_dcdx = dc_dx ;

					FormBuilder::addTauDu( forms.B, m, nidx, itp, dc_dx, nodes.indices ) ;
					FormBuilder::addTauWu( forms.J, m, nidx, itp, dc_dx, nodes.indices ) ;
					FormBuilder:: addDuDv( forms.A, w, nidx, const_dcdx.row(k0), itp, dc_dx, nodes.indices ) ;
					if( config.enforceMaxFrac ) {
						FormBuilder::addVDp  ( forms.C, m, nidx, itp, dc_dx, nodes.indices ) ;
					}

				}
			}


		}
#else
		builder.integrate_particle( particles, [&]( ID, Scalar w, const Loc&, const Itp& itp, const Dcdx& dc_dx )
			{
				FormBuilder::addTauDu( forms.B, w, itp, dc_dx, nodes.indices, nodes.indices ) ;
				FormBuilder::addTauWu( forms.J, w, itp, dc_dx, nodes.indices, nodes.indices ) ;
				FormBuilder:: addDuDv( forms.A, w, itp, dc_dx, nodes.indices, nodes.indices ) ;
				if( config.enforceMaxFrac ) {
					FormBuilder::addVDp  ( forms.C, w, itp, dc_dx, nodes.indices, nodes.indices ) ;
				}
			}
		);
#endif
		Log::Debug() << "Integrate particle: " << timer.elapsed() << std::endl ;

	}

	// Rigid bodies
	timer.reset() ;
#pragma omp parallel for if( rbData.size() > 1)
	for( unsigned k = 0 ; k < rbData.size() ; ++k )
	{
		rbData[k].assemble_matrices( nodes, m+mc ) ;
	}
	Log::Debug() << "Integrate rbs: " << timer.elapsed() << std::endl ;


	// A = mass + viscosity
#ifndef FULL_FEM
	forms.M_lumped *= config.volMass / dt  ;

	forms.A *= 2 * config.viscosity ;
	forms.A += forms.M_lumped ;
#endif


	// Projections
	const typename FormMat<3,3>::SymType IP = proj.vel.Identity() - proj.vel ;
	forms.A = proj.vel * ( forms.A * proj.vel ) + IP ;

#pragma omp parallel for
	for( ID i = 0 ; i < m ; ++i ) {
		const Scalar mass = forms.M_lumped.block(i).trace() / 3 ;
		forms.M_lumped         .block(i) = proj.vel.block(i) * mass
				+ Mat3::Identity() - proj.vel.block(i) ;
		forms.M_lumped_inv     .block(i) = proj.vel.block(i) * 1./(mass + mass_regul )
				+ Mat3::Identity() - proj.vel.block(i) ;
		forms.M_lumped_inv_sqrt.block(i) = proj.vel.block(i) * 1./std::sqrt( mass + mass_regul ) ;
	}


}

void PhaseStepData::computeAnisotropy(const DynVec &orientation, const Config& config,
									 typename FormMat<6,6>::SymType &Aniso ) const
{
	// Compute anisotropy matrix from interpolated orientation distributions

	Aniso = proj.stress.Identity() ;

	if( config.anisotropy <= 0 )
		return ;

	const ID m  = nodes.count() ;

#pragma omp parallel for
	for( ID i = 0 ; i < m ; ++i ) {

		Mat3 ori ;
		tensor_view( Segmenter<6>::segment( orientation, i ) ).get( ori ) ;

		ori = (1. - config.anisotropy) * Mat3::Identity() + 3 * config.anisotropy * ori ;

		compute_anisotropy_matrix( ori, Aniso.block(i) );

	}

}

void PhaseStepData::compute(const DynParticles& particles,
		const Config &config, const Scalar dt, Phase &phase,
		std::vector< RigidBody   >& rigidBodies,
		std::vector<TensorField > &rbStresses , std::vector<RigidBodyData> & rbData)
{
	const MeshType& mesh = phase.fraction.mesh() ;

	m_surfaceNodes.clear();
	m_surfaceNodes.resize( mesh.nNodes() );

	// Compute volumes of cells
	ScalarField volumes(mesh) ; volumes.set_zero();
	for( typename MeshType::CellIterator it = mesh.cellBegin() ; it != mesh.cellEnd() ; ++it ) {
		typename MeshType::CellGeo geo ;
		typename MeshType::NodeList nodes ;

		mesh.get_geo( *it, geo );
		const Scalar vol = geo.volume() / MeshType::NV ;

		mesh.list_nodes( *it, nodes );
		for( ID k = 0 ; k < MeshType::NV ; ++k ) {
			volumes[nodes[k]] += vol ;
		}
	}

	// Transfer particles quantities to grid
	ScalarField intPhi ( mesh ) ;
	VectorField intPhiVel     ( mesh ) ;
	ScalarField intPhiInertia ( mesh ) ;
	ScalarField intPhiCohesion( mesh ) ;
	TensorField intPhiOrient  ( mesh ) ;
	std::vector< bool > activeCells ;

#if defined(FULL_FEM)
	intPhi.set_constant( 1. ) ;
	intPhiVel.set_zero() ;
	intPhiInertia.set_zero() ;
	intPhiCohesion.set_zero() ;
	intPhiOrient.set_zero() ;
	activeCells.assign( activeCells.size(), true ) ;
#else
	particles.read( activeCells, intPhi, intPhiVel, intPhiInertia, intPhiOrient, intPhiCohesion ) ;
#endif

	// Compute phi and grad_phi (for visualization purposes )
	phase.fraction.flatten() = intPhi.flatten() ;
	phase.fraction.divide_by( volumes ) ;
	computeGradPhi( phase.fraction, volumes, phase.grad_phi ) ;

	// Active nodes
	computeActiveNodes( activeCells, phase.grad_phi ) ;
	Log::Verbose() << "Active nodes: " << nodes.count() << " / " << mesh.nNodes() << std::endl;

	//Rigid bodies and frictional boundaries
	computeActiveBodies( rigidBodies, rbStresses, rbData );

	// Bilinear forms matrices
	mesh.make_bc( StrBoundaryMapper( config.boundary ), m_surfaceNodes ) ;
	nodes.field2var( volumes, forms.volumes ) ;
	assembleMatrices( particles.geo(), config, dt, mesh, intPhi, rbData );

	nodes.field2var( phase.fraction, fraction ) ;
	nodes.field2var( intPhiVel, forms.phiu ) ;

	// Cohesion, inertia, orientation
	DynVec orientation ;
	intPhiCohesion.divide_by_positive( intPhi ) ;
	intPhiInertia .divide_by_positive( intPhi ) ;
	intPhiOrient  .divide_by_positive( intPhi ) ;

	nodes.field2var( intPhiCohesion, cohesion ) ;
	nodes.field2var( intPhiInertia , inertia  ) ;
	nodes.field2var( intPhiOrient  , orientation  ) ;

	computeAnisotropy( orientation, config, Aniso );

	// External forces
	VectorField gravity ( mesh ) ;
	gravity.set_constant( config.gravity );
	gravity.multiply_by( intPhi ) ;
	gravity.flatten() *= config.volMass ;
	nodes.field2var( gravity, forms.externalForces ) ;
}


void PhaseStepData::computeGradPhi( const ScalarField& fraction, const ScalarField& volumes, VectorField &grad_phi ) const
{
	const MeshType &mesh = grad_phi.mesh() ;

	grad_phi.set_zero() ;

	FormBuilder builder(mesh) ;

	typedef const typename MeshType::Location& Loc ;
	typedef const typename MeshType::Interpolation& Itp ;
	typedef const typename MeshType::Derivatives& Dcdx ;

	builder.integrate_qp( mesh.cellBegin(), mesh.cellEnd(),[&]( Scalar w, Loc , Itp itp, Dcdx dc_dx )
	{
		for( ID j = 0 ; j < MeshType::NV ; ++j ) {
			for( ID k = 0 ; k < MeshType::NV ; ++k ) {
				grad_phi[ itp.nodes[j] ] += w * dc_dx.row(k) * itp.coeffs[k] * fraction[ itp.nodes[k] ] ;
			}
		}
	} ) ;

	grad_phi.divide_by( volumes ) ;
}

void PhaseStepData::computeActiveNodes(const std::vector<bool> &activeCells ,
									 const VectorField &grad_phi )
{
	const MeshType &mesh = grad_phi.mesh() ;

	nodes.reset( mesh.nNodes() );

	std::vector< int > activeNodes( mesh.nNodes(), 0 ) ;

	Eigen::Matrix< Scalar, 3, Eigen::Dynamic > vecs( 3, mesh.nNodes() ) ;
	vecs.setZero() ;

	for( typename MeshType::CellIterator it = mesh.cellBegin() ; it != mesh.cellEnd() ; ++it )
	{
		if (!activeCells[ it.index() ] ) continue ;

		nodes.cells.push_back( *it );

		typename MeshType::NodeList nodes ;
		mesh.list_nodes( *it, nodes );

		for( int k = 0 ; k < MeshType::NV ; ++ k ) {
			++activeNodes[ nodes[k] ] ;
		}


	}

	for( size_t i = 0 ; i < activeNodes.size() ; ++i ) {
		if( activeNodes[i] > 0 ) {

			nodes.indices[i] = nodes.nNodes++ ;

			m_surfaceNodes[i].bc = BoundaryInfo::Interior ;
		}
	}

	nodes.computeRevIndices();
}

void PhaseStepData::computeActiveBodies( std::vector<RigidBody> &rigidBodies,
										 std::vector<TensorField> &rbStresses,
										 std::vector< RigidBodyData > &rbData
										 )
{
	rbData.clear();
	for( unsigned i = 0 ; i < rigidBodies.size() ; ++i ) {
		rbData.emplace_back( rigidBodies[i], rbStresses[i] );
	}

#pragma omp parallel for
	for( unsigned i = 0 ; i < rigidBodies.size() ; ++i ) {
		rbData[i].compute_active( nodes, m_surfaceNodes ) ;
		rbData[i].nodes.computeRevIndices() ;
	}

	m_totRbNodes = 0 ;
	for( unsigned i = 0 ; i < rigidBodies.size() ; ++i )
	{
		rbData[i].nodes.setOffset( m_totRbNodes + nodes.count() ) ;
		m_totRbNodes += rbData[i].nodes.count() ;
	}
	Log::Debug() << "Tot coupling nodes: " << nSuppNodes() << std::endl ;



}


} //mpm
