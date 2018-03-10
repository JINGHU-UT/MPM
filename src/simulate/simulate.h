
#ifndef MPM_SIMU_HH
#define MPM_SIMU_HH

#include "geometry/geometry.fwd.h"

#include "simulate/dynParticles.h"
#include "simulate/phaseSolver.h"

#include "utils/stats.h"

#include <memory>

namespace mpm {

struct Config ;
struct Phase ;
class RigidBody ;
class Scenario ;

class Simulate {

public:

	explicit Simulate( const Config& config, const char* base_dir ) ;
	~Simulate() ;

	void run() ;

	MeshType& mesh() { return * m_mesh ;  }
	const MeshType& mesh() const { return * m_mesh ;  }

	std::vector< RigidBody > &rigidBodies ()
	{
		return m_rigidBodies ;
	}
	const std::vector< RigidBody > &rigidBodies () const
	{
		return m_rigidBodies ;
	}

	DynParticles& particles() { return m_particles ; }
	const DynParticles& particles() const { return m_particles ; }

private:
	void step( const Scalar dt ) ;

	void dump_fields(unsigned frame) const ;
	void dump_particles(unsigned frame) const ;

	Simulate( const Simulate& ) = delete ;
	Simulate& operator=( const Simulate& ) = delete ;


	const Config& m_config ;
	const char* m_base_dir ;

	Stats		  m_stats ;
	DynParticles  m_particles ;
	std::unique_ptr<Scenario>  m_scenario ;

	std::vector< RigidBody   > m_rigidBodies ;

	std::unique_ptr<MeshType>  m_mesh ;
	std::unique_ptr<Phase>     m_grains ;
	// Useful for warm-starting stresses at frictional boundary conditions
	std::vector< TensorField > m_rbStresses  ;

	PhaseSolver m_solver ;
};

} //mpm

#endif
