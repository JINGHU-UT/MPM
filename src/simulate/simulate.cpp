
#include "simulate.h"
#include "phase.h"

#include "scenario.h"
#include "rigidBody.h"

#include "utils/log.h"
#include "utils/file.h"
#include "utils/config.h"

#include "geometry/mesh.impl.h"
#include "geometry/levelSet.h"

#include "geometry/levelSet.io.h"
#include "geometry/particles.io.h"

#include <boost/archive/binary_oarchive.hpp>
#include <bogus/Core/Utils/Timer.hpp>
#include <bogus/Core/Eigen/EigenSerialization.hpp>

namespace mpm {


Simulate::Simulate(const Config &config, const char *base_dir)
	: m_config(config), m_base_dir( base_dir ),
	  m_stats( m_base_dir ),
	  m_scenario( Scenario::parse( config ) ),
	  m_mesh( new MeshImpl( config.box, config.res ) ),
	  m_grains( new Phase( mesh() ) ),
	  m_solver( m_particles )
{

	m_particles.generate( config, mesh(), *m_scenario );

	// Rigid bodies
	m_scenario->add_rigid_bodies( m_rigidBodies ) ;

	for( unsigned i = 0 ; i < m_rigidBodies.size() ; ++i ) {
		m_rbStresses.emplace_back( mesh() );
		m_rbStresses.back().set_zero() ;
	}
}

Simulate::~Simulate()
{
}

void Simulate::run()
{
	m_grains->fraction.set_zero();
	m_grains->stresses.set_zero();
	m_grains->velocity.set_zero();
	m_grains->sym_grad.set_zero();
	m_grains->spi_grad.set_zero();
	m_grains->geo_proj.set_zero();

	if( m_config.output ) {
		dump_particles( 0 ) ;
		dump_fields( 0 ) ;
	}
	m_particles.events().start();

	for( unsigned frame = 0 ; frame < m_config.nFrames ; ++ frame ) {
		// bogus::Timer timer ;
		Log::Info() << "Starting frame " << (frame+1) << std::endl ;

		unsigned substeps = m_config.substeps ;
		if( substeps == 0 ) { //Adaptative timestepping
			substeps = std::ceil( std::max(1., m_stats.maxVelocity) / m_config.fps ) ;
		}

		for( unsigned s = 0 ; s < substeps ; ++ s ) {

			m_stats.frameId = frame ;
			m_stats.delta_t = 1./(m_config.fps * substeps) ;

			const Scalar t = m_config.time( frame ) + s * m_stats.delta_t ;
			Log::Verbose() << arg3( "Step %1/%2 \t t=%3 s",
									s+1, substeps,
									t * m_config.units().toSI( Units::Time ) ) << std::endl ;
			// Update external objects (moving boundaries,...)
			m_scenario->update( *this, t, m_stats.delta_t ) ;

			// Dump particles at last substep of each frame
			if( m_config.output && substeps == s+1 ) {
				dump_particles( frame+1 ) ;
				m_particles.events().clear();
			}
			m_particles.events().start();

			// Proper simulation step
			step( m_stats.delta_t ) ;

			// Log max particle velocity (useful for adaptative timestep )
			m_stats.maxVelocity = m_particles.geo().velocities().leftCols(m_particles.count()).lpNorm< Eigen::Infinity >() ;
			Log::Debug() << "Max particle vel: " << m_stats.maxVelocity << std::endl ;

			m_stats.dump();
		}

		Log::Info() << "Frame done !" << std::endl ;

		if( m_config.output ) {
			dump_fields( frame+1 ) ;
		}
	}

	Log::Info() << "All done." << std::endl ;

}

void Simulate::step(const Scalar dt)
{
	// bogus::Timer timer ;

	// TODO: if we were to adapt the mesh, it would be here

	m_stats.nParticles = m_particles.count() ;
	m_stats.nNodes = m_mesh->nNodes() ;

	//! Compute new grid velocities
	m_solver.step( m_config, dt, *m_grains, m_stats, m_rigidBodies, m_rbStresses ) ;
	// const Scalar solverTime = timer.elapsed() ;

	//! Advance particles
	m_particles.update( m_config, dt, *m_grains ) ;

	for( RigidBody& rb: m_rigidBodies ) {
		rb.move( dt );
	}

	// m_stats.totalTime = timer.elapsed() ;
	// m_stats.advectionTime = timer.elapsed() - solverTime ;

	Log::Verbose() << "Step done " << std::endl ;
}

void Simulate::dump_particles( unsigned frame ) const
{
	FileInfo dir( FileInfo(m_base_dir).filePath( arg("frame-%1", frame ) ) ) ;
	dir.makePath() ;
	if( ! dir.exists() )
		dir.makeDir() ;

	// Particles
	{
		std::ofstream ofs( dir.filePath("particles") );
		boost::archive::binary_oarchive oa(ofs);
		oa << m_particles.geo() ;
	}
	// Objects
	{
		std::ofstream ofs( dir.filePath("objects") );
		boost::archive::binary_oarchive oa(ofs);

		unsigned n = m_rigidBodies.size() ;
		oa << n ;
		LevelSet::register_derived(oa) ;
		for( unsigned i = 0 ; i < n ; ++i ) {
			const LevelSet* ptr = m_rigidBodies[i].levelSetPtr() ;
			oa << ptr ;
		}
	}

	// Log -- save at previous frame
	if( frame > 0 ) {
		FileInfo dir( FileInfo(m_base_dir).filePath( arg("frame-%1", frame-1 ) ) );
		std::ofstream ofs( dir.filePath("log") );
		boost::archive::binary_oarchive oa(ofs);
		oa << m_particles.events() ;
	}
}


void Simulate::dump_fields( unsigned frame ) const
{
	FileInfo dir( FileInfo(m_base_dir).filePath( arg("frame-%1", frame ) ) ) ;
	dir.makePath() ;
	if( ! dir.exists() )
		dir.makeDir() ;

	// Grid
	{
		std::ofstream ofs( dir.filePath("mesh") );
		boost::archive::binary_oarchive oa(ofs);
		oa << m_mesh->derived() ;
	}
	// Velocity, Stress, Phi
	{
		std::ofstream ofs( dir.filePath("fields") );
		boost::archive::binary_oarchive oa(ofs);
		oa << *m_grains ;
	}
}


} //mpm
