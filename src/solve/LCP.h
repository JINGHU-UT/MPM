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

#ifndef MPM_LCP_H_
#define MPM_LCP_H_

#include "utils/block_mat.h"

namespace  mpm {

struct LCPData {
	typedef typename FormMat<3,1>::Type HType ;

	HType H   ;
	DynVec w  ;

	ID n() const { return H.colsOfBlocks() ; }
};

class LCP {

public:
	LCP( const LCPData &data ) ;

	Scalar solve( DynVec& x ) const ;

private:
	const LCPData& m_data ;

};

} //mpm

#endif

