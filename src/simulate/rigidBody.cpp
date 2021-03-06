


#include "rigidBody.h"



#include <iostream>

namespace mpm {

RigidBody::RigidBody(std::unique_ptr<LevelSet> &ls, Scalar volMass )
		: m_levelSet ( std::move(ls) ), m_volumicMass( volMass )
{
	m_velocity.setZero() ;
}

Vec3 RigidBody::velocity_at(const Vec3 &x) const
{
	return velocity() + angularVelocity().cross( x ) ;
}

void RigidBody::integrate_forces(const Scalar dt, const Vec6 &forces)
{
	Mat6 Mi ;
	inv_inertia( Mi );

	m_velocity += dt * Mi * forces ;
}

void RigidBody::integrate_gravity(const Scalar dt, const Vec3 &gravity)
{
	Vec6 forces ;
	forces.head<3>() = m_volumicMass * m_levelSet->volume() * gravity ;
	forces.tail<3>().setZero() ;

	integrate_forces( dt, forces );
}

void RigidBody::move(const Scalar dt) const
{

	const Scalar avn = angularVelocity().norm() ;
	Vec3 axis = avn > 1.e-12
			? Vec3(angularVelocity()/avn)
			: Vec3(1,0,0) ;
	Eigen::AngleAxis< Scalar > aa( dt * avn, axis ) ;

	m_levelSet->move( dt * velocity(), Quaternion( aa ) );
}

void RigidBody::move_to(const Vec3 &pos) const
{
	m_levelSet->set_origin( pos ) ;
}

void RigidBody::inv_inertia( Mat6& Mi ) const
{
	m_levelSet->inv_inertia( Mi ) ;
	Mi /= m_volumicMass ;
}

} //mpm


