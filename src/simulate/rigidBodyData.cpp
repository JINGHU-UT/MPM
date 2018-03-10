
#include "rigidBodyData.h"


namespace mpm {

const Scalar RigidBodyData::s_splatRad = 1 ;

RigidBodyData::RigidBodyData( RigidBody& rb_, TensorField &s )
	: rb(rb_), stresses(s)
{
}

Scalar RigidBodyData::phi( const Vec3 &x ) const
{
	return std::min( 1., std::max(0., 1. + rb.levelSet().eval_at( x ) / s_splatRad ) );
}
void RigidBodyData::grad_phi(const Vec3 &x, Vec3 &grad) const
{
	rb.levelSet().grad_at( x, grad );
	grad /= s_splatRad ;
}

void RigidBodyData::compute_active( const Active& phaseNodes, BoundaryConditions& bc )
{
	const MeshType &mesh = stresses.mesh() ;

	typename MeshType::NodeList nodelist ;
	typename MeshType::CellGeo geo ;

	nodes.reset( mesh.nNodes() );

	for( const typename MeshType::Cell& cell : phaseNodes.cells )
	{
		mesh.get_geo( cell, geo );

		bool occupied = false ;
		bool boundary = false ;
		bool interior = true  ;

		for( ID k = 0 ; k < MeshType::NV ; ++k  ) {
			const Scalar phi_k = phi( geo.vertex(k) ) ;

			if( phi_k >= 1. ) {
				boundary = true ;
			} else {
				if( phi_k >  0. ) {
					occupied = true ;
				}
				interior = false ;
			}
		}

		if( occupied || boundary ) {
			occupiedCells.push_back( cell ) ;
		}

		if( boundary && !interior )
		{
			nodes.cells.push_back( cell ) ;
			mesh.list_nodes( cell, nodelist );

			for( ID k = 0 ; k < MeshType::NV ; ++k  ) {
				if( nodes.indices[ nodelist[k] ] == Active::s_Inactive ) {
					nodes.indices[ nodelist[k] ] = nodes.nNodes++ ;
					bc[ nodelist[k] ].bc = BoundaryInfo::Interior ;
				}
			}
		}
	}
}

void RigidBodyData::integrate(const Active &phaseNodes, ID totNodes)
{
	typedef const typename MeshType::Location& Loc ;
	typedef const typename MeshType::Interpolation& Itp ;
	typedef const typename MeshType::Derivatives& Dcdx ;

	const ID m = phaseNodes.count() ;

	const MeshType &mesh = stresses.mesh() ;
	FormBuilder builder( mesh ) ;
	builder.reset( totNodes );
	builder.addToID( occupiedCells, phaseNodes.indices, phaseNodes.indices );
	builder.addToID(   nodes.cells,      nodes.indices, phaseNodes.indices );
	builder.makeCompressed();

	jacobian.clear() ;
	jacobian.setRows( totNodes );
	jacobian.setCols( m );
	jacobian.cloneIndex( builder.index() ) ;
	jacobian.setBlocksToZero() ;

	builder.integrate_qp( occupiedCells, [&]( Scalar w, Loc loc, Itp itp, Dcdx )
	{
		Vec3 dphi_dx ;
		grad_phi( mesh.pos( loc ), dphi_dx ) ;

		FormBuilder:: addUTaunGphi( jacobian, w, itp, dphi_dx, phaseNodes.indices, phaseNodes.indices ) ;
	}
	);
	builder.integrate_node(  nodes.cells, [&]( Scalar w, Loc loc, Itp itp )
	{
		Vec3 dphi_dx ;
		grad_phi( mesh.pos( loc ), dphi_dx ) ;

		FormBuilder:: addUTauGphi ( jacobian, w, itp, dphi_dx,      nodes.indices, phaseNodes.indices ) ;
	}
	);

}

void RigidBodyData::assemble_matrices(const Active &phaseNodes, ID totNodes)

{
	const MeshType &mesh = stresses.mesh() ;

	typename MeshType::NodeList nodelist ;
	typename MeshType::CellGeo geo ;

	fraction.resize( nodes.count() );
	fraction.setConstant( -1 ) ;

	typename FormMat<3,6>::UncompressedType	proj ;
	proj.clear() ;
	proj.setRows( phaseNodes.count() );
	proj.setCols( 1 );

	typename FormMat<3,6>::BlockT P ;
	P.block<3,3>(0,0).setIdentity() ;

	for( const typename MeshType::Cell& cell : nodes.cells )
	{
		mesh.get_geo( cell, geo );
		mesh.list_nodes( cell, nodelist );

		for( ID k = 0 ; k < MeshType::NV ; ++k  ) {
			const ID glb_idx = nodelist[k] ;
			const ID loc_idx = nodes.indices[glb_idx] - nodes.offset ;
			const Vec3 pos = geo.vertex(k) ;

			if( fraction( loc_idx ) < 0. ) {
				fraction( loc_idx ) = phi( pos ) ;
				const Vec3 dx = pos - rb.levelSet().origin() ;
				make_cross_mat( dx, P.block<3,3>(0,3) ) ;
				proj.insert( phaseNodes.indices[ glb_idx ], 0  ) = P ;
			}
		}
	}


	proj.finalize() ;

	projection = proj.transpose() ;

	integrate( phaseNodes, totNodes );
}


} //mpm
