
/*
 * This file is part of Sand6, a C++ software for the continuous
 * simulation of granular materials. See http://gdaviet.fr
 *
 * Copyright 2016 Gilles Daviet <gilles.daviet@inria.fr>
 *
 * Sand6 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.

 * Sand6 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with Sand6.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Offline.hh"

#include "simulate/phase.h"

#include "geometry/mesh.impl.h"
#include "utils/log.h"
#include "utils/file.h"

#include "geometry/particles.io.h"
#include "geometry/levelSet.io.h"

#include <boost/archive/binary_iarchive.hpp>

#include <bogus/Core/Eigen/EigenSerialization.hpp>

namespace mpm {

Offline::Offline(const char *base_dir)
	: m_base_dir( base_dir ),
	  m_mesh( new MeshImpl( Vec3::Ones(), Vec3i::Ones() ) ),
	  m_grains( new Phase( *m_mesh ) )
{
	m_config.from_file( FileInfo( base_dir ).filePath( "config" ) ) ;
	m_config.internalize() ;
	Log::Info() << arg( "Frame Dt is %1 (IU) = %2 FPS (SI)", frame_dt(), m_config.fps / m_config.units().toSI( Units::Time )) << std::endl ;;
}

Offline::~Offline(){
}

bool Offline::load_frame(unsigned frame )
{
	FileInfo dir( FileInfo(m_base_dir).filePath( arg("frame-%1", frame ) ) ) ;
	if( !dir.exists() ) {
		Log::Error() << "Directory does not exist: " << dir.path() << std::endl ;
		return false ;
	}

	try {

		// Grid
		{
			std::ifstream ifs( dir.filePath("mesh") );
			boost::archive::binary_iarchive ia(ifs);
			ia >> m_mesh->derived() ;
		}
		// Velocity, Stress, Phi
		{
			std::ifstream ifs( dir.filePath("fields") );
			boost::archive::binary_iarchive ia(ifs);
			ia >> *m_grains ;
		}
		// Particles
		{
			std::ifstream ifs( dir.filePath("particles") );
			boost::archive::binary_iarchive ia(ifs);
			ia >> m_particles ;
		}
		// Log
		{
			m_events.clear() ;
			std::ifstream ifs( dir.filePath("log") );
			if(ifs) {
				boost::archive::binary_iarchive ia(ifs);
				ia >> m_events ;
			}
		}

		//Objects
		{
			m_levelSets.clear();

			std::ifstream ifs( dir.filePath("objects") );
			boost::archive::binary_iarchive ia(ifs);

			unsigned n = 0 ;
			ia >> n ;
			LevelSet::register_derived(ia) ;
			for( unsigned i = 0 ; i < n ; ++i ) {
				LevelSet* ptr ;
				ia >> ptr ;
				m_levelSets.emplace_back( ptr ) ;
			}

		}

	} catch (boost::archive::archive_exception &e) {
		Log::Error() << "Error reading frame data: " << e.what() << std::endl ;
		return false ;
	}

	Log::Info() << "Loaded frame " << frame << std::endl ;

	return true ;

}

} //mpm
