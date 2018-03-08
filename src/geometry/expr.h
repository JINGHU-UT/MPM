

#ifndef mpm_expr_h_
#define mpm_expr_h_



#include "utils/alg.h"
#include "geometry.fwd.h"

namespace mpm{

  template <typename ValueType>
  struct Expr
  {
  	virtual ValueType operator() ( Vec3 const& x ) const = 0 ; 
  };

  typedef Expr< Scalar > ScalarExpr ;

  
  

}


#endif

