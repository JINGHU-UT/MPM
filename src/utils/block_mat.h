#ifndef MPM_BLOCK_MAT_H_
#define MPM_BLOCK_MAT_H_

#include "utils/alg.h"
#include <bogus/Core/Block.hpp>

namespace mpm {

template < ID Rows, ID Cols >
struct FormMat {
	typedef Eigen::Matrix< Scalar, Rows, Cols > BlockT ;
	typedef bogus::SparseBlockMatrix< BlockT > Type ;
	typedef bogus::SparseBlockMatrix< BlockT, bogus::UNCOMPRESSED > UncompressedType ;
	typedef bogus::SparseBlockMatrix< BlockT, bogus::SYMMETRIC > SymType ;
};

} // mpm

#endif
