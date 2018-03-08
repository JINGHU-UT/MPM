
#ifndef MPM_PARTICLES_IO_HH
#define MPM_PARTICLES_IO_HH

#include "particles.h"

#include <boost/serialization/array.hpp>
#include <boost/serialization/vector.hpp>

namespace mpm {

template < typename Archive >
void Particles::serialize( Archive &ar, unsigned int ) {
	using boost::serialization::make_array ;

	ar & m_count ;
	ar & make_array( m_volumes.data(),      m_count ) ;
	ar & make_array( m_centers.data(),    3*m_count ) ;
	ar & make_array( m_velocities.data(), 3*m_count ) ;
	ar & make_array( m_frames.data(),     6*m_count ) ;
	ar & make_array( m_orient.data(),     6*m_count ) ;
}

template < typename Archive >
void Particles::Event::serialize( Archive &ar, unsigned int ) {
	ar & type ;
	ar & first ;
	ar & second ;
	ar & dx ;
}

}//mpm

#endif

