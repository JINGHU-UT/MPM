
#ifndef MPM_MESH_TRAITS_H_
#define MPM_MESH_TRAITS_H_

#include "voxel.h"
#include "tet.h"

#include <vector>

namespace mpm {

template <typename Derived >
struct MeshTraits {
};

class  Grid ;
struct GridIterator ;

template < >
struct MeshTraits< Grid > {
	typedef GridIterator CellIterator ;

	typedef Vec3i Cell    ;
	typedef Voxel CellGeo ;
	static constexpr ID NV = CellGeo::NV ;
	static constexpr ID NC = CellGeo::NC ;
	static constexpr ID NQ = CellGeo::NQ ;

	typedef std::vector<Cell> Cells ;
};

class  TetGrid ;
struct TetGridIterator ;

template < >
struct MeshTraits< TetGrid > {
	typedef TetGridIterator CellIterator ;

	typedef Eigen::Array< int, 4, 1 > Cell  ;
	typedef Tet CellGeo ;
	static constexpr ID NV = CellGeo::NV ;
	static constexpr ID NC = CellGeo::NC ;
	static constexpr ID NQ = CellGeo::NQ ;

	typedef std::vector<Cell> Cells ;
};


} //mpm

#endif
