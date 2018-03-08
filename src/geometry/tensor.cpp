

#include "tensor.h"

namespace mpm{

  void compute_anisotropy_matrix( const Mat3& A, Mat6 & Abar )
  {

    Abar.setIdentity() ;

    Vec6 taubar ;
    Mat3 tau, ani_tau ;

    for( int k = 1 ; k < 6 ; ++k ) {
      taubar.setZero() ;
      taubar[k] = 1. ;

      tensor_view( taubar ).get( tau ) ;
      ani_tau = A * tau * A ;

      tensor_view( Abar.col(k) ).set( ani_tau );
      Abar(0,k) = 0 ;
    }

  }







} // namespace mpm