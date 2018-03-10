
#ifndef MPM_PHASE_SOLVE_H_
#define MPM_PHASE_SOLVE_H_

#include "activeIndices.h"

#include "rigidBody.h"
#include "rigidBodyData.h"

#include "phase.h"
#include "phaseStepData.h"

#include "formBuilder.h"
#include "linearSolver.h"

#include "geometry/boundaryInfo.h"
#include "geometry/meshBase.h"

#include "solve/PrimalData.h"
#include "solve/Primal.h"
#include "solve/LCP.h"

#include "utils/scalar.h"
#include "utils/log.h"
#include "utils/config.h"
#include "utils/stats.h"

#include <bogus/Core/Block.impl.hpp>
#include <bogus/Core/Utils/Timer.hpp>

#include <vector>

namespace mpm {

class DynParticles ;
struct Phase ;
struct PhaseStepData ;

class RigidBody ;
struct RigidBodyData ;
struct PrimalData ;

struct Config ;
class Stats ;

class PhaseSolver {

public:
	explicit PhaseSolver(
			const DynParticles& particles
			) ;

	//! Solve for end-of-steps velocities, reading initial quantities from the particles
	void step(const Config &config, const Scalar dt,
			  Phase& phase, Stats &stats,
			  std::vector<RigidBody> &rigidBodies,
			  std::vector<TensorField> &rbStresses
			  ) const ;

private:
	//! Two-steps solve of the momentum balance w/ frictional rheology
	void solve(const Config& config, const Scalar dt,
			   const PhaseStepData &stepData,
			   Phase& phase, std::vector< RigidBodyData > &rbData, Stats& stats ) const ;

	//! Assemble and solve the friction problem
	void solveComplementarity(const Config&c, const Scalar dt,
							  const PhaseStepData& stepData ,
							  std::vector< RigidBodyData >& rbData,
							  DynVec &u, Phase &phase, Stats &stats ) const ;

	//! Add contbutions from a rigid body to the friction problem
	void addRigidBodyContrib(const Config &c, const Scalar dt, const PhaseStepData &stepData,
							 const DynVec &u, const RigidBodyData &rb,
							 PrimalData& primalData, DynArr &totFraction ) const ;
	//! Add contribution from chesive forces to the friction problem
	void addCohesionContrib (const Config&c, const PhaseStepData &stepData,
							  PrimalData& primalData, DynVec &u ) const ;

	//! Compute displacement for volume correction
	void enforceMaxFrac(const Config &c, const PhaseStepData &stepData,
									   const std::vector<RigidBodyData> &rbData,
									   DynVec &depl ) const ;


	const DynParticles& m_particles ;

};


} //mpm


#endif
