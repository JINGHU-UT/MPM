

#include "voxel.h"

#include "tensor.h"



namespace mpm {
  
  Index Voxel::sample_uniform(const unsigned N, const Index start, Points &points, Frames &frames) const
  {
    Scalar min = box.minCoeff() ;
  
    Vec3i Nsub ;
    for( int k = 0 ; k < 3 ; ++ k)
      Nsub[k] = N * std::round( box[k] / min ) ;
  
    const Vec3 subBox = box.array() / Nsub.array().cast< Scalar >() ;
  
    Vec6 frame ;
    tensor_view( frame ).set_diag( Vec3( .25 * subBox.array() * subBox.array() ) ) ;
  
    Index p = start ;
    for( int i = 0 ; i < Nsub[0] ; ++i )
      for( int j = 0 ; j < Nsub[1] ; ++j )
        for( int k = 0 ; k < Nsub[2] ; ++k ) {
          points.col(p) = origin + (Vec3(i+.5,j+.5,k+.5).array() * subBox.array()).matrix() ;
          frames.col(p) = frame ;
          ++p ;
        }
  
    return p - start ;
  }
  
  Voxel::QuadPoints Voxel::Qps()
  {
    // .5 * ( 1 +- 1./sqrt(3) )
    const Vec3 dqp = Vec3::Constant( 1./sqrt(3.) );
    //    const Vec dqp = Vec::Constant( 1. );
    const Vec3 qp0 = .5 * ( Vec3::Ones() - dqp );
  
    QuadPoints qps ;
    for( int i = 0 ; i < 2 ; ++i ) {
      for( int j = 0 ; j < 2 ; ++j ) {
        for( int k = 0 ; k < 2 ; ++k ) {
          Vec3i corner ( i, j, k) ;
          qps.col( cornerIndex(corner) ) = qp0.array() + corner.cast< Scalar >().array()*dqp.array() ;
        }
      }
    }
    return qps ;
  }

} 