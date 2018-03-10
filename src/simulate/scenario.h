
#ifndef MPM_SCENARIO_H_
#define MPM_SCENARIO_H_

#include "utils/scalar.h"
#include "utils/units.h"
#include "utils/config.h"
#include "utils/string.hh"
#include "utils/log.h"

#include "scenario.h"
#include "rigidBody.h"

#include "geometry/expr.h"
#include "geometry/levelSet.h"


#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace mpm {

struct Config ;
struct ScenarioFactory ;
class RigidBody ;
class Simulate ;

class Scenario {

public:

	static std::unique_ptr< Scenario > parse( const Config& config ) ;
	static void register_factory( const ScenarioFactory& factory ) ;

	virtual ~Scenario() {}

	struct ParticleGenerator : public Expr<Scalar> {
		const Scenario& scenario ;
		ParticleGenerator( const Scenario& s ) : scenario(s) {}
		Scalar operator() ( const Vec3&  x ) const { return scenario.particle_density(x) ; }
	};
	ParticleGenerator generator() const { return ParticleGenerator(*this) ; }

	virtual Scalar particle_density( const Vec3 &x ) const = 0 ;

	virtual void add_rigid_bodies( std::vector< RigidBody >& /*rbs*/ ) const {}
	virtual void update( Simulate& /*simu*/, Scalar /*time*/, Scalar /*dt*/ ) const {}

protected:
	typedef std::unordered_map< std::string, std::string > Params ;

	Scenario() : m_config(0) {}

	virtual void init( const Params& /*params*/ ) {}

	Scalar      scalar_param( const Params& params, const std::string& key,
						 Units::Unit unit = Units::None,
						 Scalar def = 0. ) const ;
	std::string string_param( const Params& params, const std::string& key,
						 const std::string& def = "" ) const ;

	const Config* m_config ;

};

struct ScenarioFactory {
	virtual std::unique_ptr< Scenario > make( const std::string& str ) const = 0 ;

	ScenarioFactory() {}
	ScenarioFactory( const ScenarioFactory& ) = delete ;
	ScenarioFactory& operator= ( const ScenarioFactory& ) = delete ;
};


} //mpm

#endif
