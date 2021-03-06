

#include "levelSet.h"

#include "levelSet.impl.h"

#include <iostream>

namespace mpm {

LevelSet::LevelSet()
	: m_origin(Vec3::Zero()), m_frame( Quaternion::Identity() )
{
	scale(1.) ;
}

void LevelSet::move(const Vec3 &depl, const Quaternion &rot)
{
	m_origin += depl ;
	m_frame   = rot * m_frame ;
}

Scalar LevelSet::eval_at(const Vec3 &x) const
{
	Vec3 loc;
	to_local(x, loc);
	return m_scale * eval_local( loc ) ;
}

void LevelSet::grad_at(const Vec3 &x, Vec3 &grad) const
{
	Vec3 loc;
	to_local(x, loc);
	grad = m_frame * ( grad_local( loc ).array() ) ;
}

void LevelSet::to_local(const Vec3 &world, Vec3 &local) const
{
//	Quaternion fi = m_frame ; fi.w() = -fi.w() ;
	local = ( m_frame.inverse() * ( world - m_origin ) ) / m_scale  ;
}
void LevelSet::to_local_mat( Mat3 &mat) const
{
	mat = m_frame.inverse().matrix() / m_scale ;
}

void LevelSet::inv_inertia(Mat6 &Mi) const
{
	Mi.setIdentity() ;
	Mi.diagonal().head<3>() /= local_volume() ;

	Mat3 I ;
	local_inv_inertia( I );
	Mat3 w2l ;
	to_local_mat( w2l );

	Mi.block<3,3>(3,3) = w2l.transpose() * I * w2l ;


	Mi *= std::pow( m_scale, -3. ) ;
}

LevelSet::Ptr LevelSet::make_sphere() { return Ptr( new SphereLevelSet() ) ; }
LevelSet::Ptr LevelSet::make_cylinder(Scalar height) { return Ptr( new CylinderLevelSet( height ) ) ; }
LevelSet::Ptr LevelSet::make_plane() { return Ptr( new PlaneLevelSet() ) ; }
LevelSet::Ptr LevelSet::make_torus(Scalar radius) { return Ptr( new TorusLevelSet( radius ) ) ; }
LevelSet::Ptr LevelSet::make_hole(Scalar radius) { return Ptr( new HoleLevelSet( radius ) ) ; }
LevelSet::Ptr LevelSet::make_hourglass(Scalar height, Scalar radius) { return Ptr( new HourglassLevelSet( height, radius ) ) ; }
LevelSet::Ptr LevelSet::from_mesh(const char* objFile) { return Ptr( new MeshLevelSet( objFile ) ) ; }

} //mpm



