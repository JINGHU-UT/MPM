#ifndef mpm_LINEAR_SOLVER_H
#define mpm_LINEAR_SOLVER_HH

#include "utils/alg.h"

#include "formBuilder.h"
#include <bogus/Core/Block.fwd.hpp>


namespace mpm {

template < typename Derived, typename OtherDerived >
Scalar solveSDP( const bogus::SparseBlockMatrixBase< Derived >& M,
						  const bogus::SparseBlockMatrixBase< OtherDerived >& P,
						  const DynVec &lhs,
						  DynVec &res  ) ;


} //mpm


#endif




