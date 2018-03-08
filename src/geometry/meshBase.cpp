
#include "meshBase.h"

#include "grid.h"
#include "tetGrid.h"

namespace mpm {

template <typename Derived>
Vec3 MeshBase< Derived>::pos( const Location& loc ) const
{
	CellGeo geo ;
	get_geo( loc.cell, geo ) ;
	return geo.pos( loc.coords ) ;
}

template class MeshBase< Grid > ;
template class MeshBase< TetGrid > ;


}  //mpm
