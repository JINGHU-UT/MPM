
#ifndef MPM_MESH_CELL_H_
#define MPM_MESH_CELL_H_

#include "geometry.fwd.h"

#if( MPM_MESH_IMPL == MPM_MESH_TET_GRID )
#include "tet.h"
#else
#include "voxel.h"
#endif

#endif
