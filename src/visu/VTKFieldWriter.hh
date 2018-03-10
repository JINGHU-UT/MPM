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

#ifndef MPM_VTK_FIELD_WRITER_HH
#define MPM_VTK_FIELD_WRITER_HH

#include "VTKWriter.hh"

#include "utils/alg.h"
#include "geometry/geometry.fwd.h"

namespace mpm {


class VTKFieldWriter : public VTKWriter
{

public:

	VTKFieldWriter( const char* base_dir, const MeshType& mesh ) ;

	template< typename Derived >
	bool dump( const char* name, const FieldBase< Derived >& field ) ;

protected:
	void writeMesh(File &file) const ;
	size_t nDataPoints( ) const ;

private:

	const MeshType& m_mesh ;

};

} //mpm

#endif

