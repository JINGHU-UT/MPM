



#ifndef mpm_TENSOR_H_
#define mpm_TENSOR_H_

#include "utils/alg.h"

namespace mpm {

static constexpr Scalar s_sqrt_2  = M_SQRT2 ;
static constexpr Scalar s_sqrt_3  = 1.73205080756887729352744634151 ;
static constexpr Scalar s_sqrt_6  = s_sqrt_2 * s_sqrt_3 ;
static constexpr Scalar s_isqrt_3 = 1./s_sqrt_3;
static constexpr Scalar s_sqrt_23 = s_sqrt_2 / s_sqrt_3 ;


template< typename Derived >
struct TensorView
{
  static constexpr ID Size       = Derived::SizeAtCompileTime ;
  static constexpr bool  HasSymPart = ( Size == 6 || Size == 9 ) ;
  static constexpr bool  HasSpiPart = ( Size == 3 || Size == 9 ) ;
  static constexpr ID SpiOff     = HasSymPart ? 6 : 0 ;

  explicit TensorView( Derived map )
    : m_map( map )
  {
    EIGEN_STATIC_ASSERT_VECTOR_ONLY(Derived);
  }

  TensorView& set_diag( const Vec3& diag ) {
    if ( HasSymPart ) {
      m_map.setZero() ;

      m_map[0] = diag.sum() / s_sqrt_6 ;
      m_map[1] = ( diag[0] - diag[1] ) / 2. ;
      m_map[2] = ( 2*diag[2] - diag[0] - diag[1] ) / (2 * s_sqrt_3) ;
    }
    return *this ;
  }

  TensorView& set( const Mat3& mat ) {

    if ( HasSymPart ) {
      m_map[0] = ( mat(0,0)+mat(1,1)+mat(2,2) ) / s_sqrt_6 ;
      m_map[1] = ( mat(0,0) - mat(1,1) ) / 2. ;
      m_map[2] = ( 2*mat(2,2) - mat(1,1) - mat(0,0) ) / (2 * s_sqrt_3) ;

      m_map[3] = .5*( mat(0,1) + mat(1,0) ) ;
      m_map[4] = .5*( mat(0,2) + mat(2,0) ) ;
      m_map[5] = .5*( mat(1,2) + mat(2,1) ) ;
    }

    if( HasSpiPart ) {
      m_map[SpiOff+0] = .5*( mat(0,1) - mat(1,0) ) ;
      m_map[SpiOff+1] = .5*( mat(0,2) - mat(2,0) ) ;
      m_map[SpiOff+2] = .5*( mat(1,2) - mat(2,1) ) ;
    }
    return *this ;
  }

  void get( Mat3& mat ) const {
    mat.setZero() ;
    add( mat ) ;
  }
  void add( Mat3& mat ) const {

    if ( HasSymPart ) {
      mat(0,0) += 2./s_sqrt_6 * m_map[0] + m_map[1] - 1./s_sqrt_3 * m_map[2] ;
      mat(1,1) += 2./s_sqrt_6 * m_map[0] - m_map[1] - 1./s_sqrt_3 * m_map[2] ;
      mat(2,2) += 2./s_sqrt_6 * m_map[0]            + 2./s_sqrt_3 * m_map[2] ;
      mat(0,1) += mat(1,0) = m_map[3] ;
      mat(0,2) += mat(2,0) = m_map[4] ;
      mat(1,2) += mat(2,1) = m_map[5] ;
    }

    if( HasSpiPart ) {
      mat(0,1) += m_map[SpiOff+0] ;
      mat(1,0) -= m_map[SpiOff+0] ;
      mat(0,2) += m_map[SpiOff+1] ;
      mat(2,0) -= m_map[SpiOff+1] ;
      mat(1,2) += m_map[SpiOff+2] ;
      mat(2,1) -= m_map[SpiOff+2] ;
    }
  }

  Mat3 as_mat() const {
    Mat3 mat ;
    get( mat ) ;
    return mat ;
  }

private:

  Derived m_map ;

};




template< typename Derived >
TensorView< Derived > tensor_view( const Eigen::MapBase< Derived, Eigen::ReadOnlyAccessors >& map )
{
  return TensorView< Derived > ( map.derived() ) ;
}

template< typename Derived >
TensorView< Derived > tensor_view( const Eigen::MapBase< Derived, Eigen::WriteAccessors >& map )
{
  return TensorView< Derived > ( map.derived() ) ;
}

template< typename Derived >
TensorView< typename Derived::MapType > tensor_view( Eigen::MatrixBase< Derived >& vec )
{
  return tensor_view( Derived::Map(vec.derived().data(), vec.size()) ) ;
}

template< typename Derived >
TensorView< typename Derived::ConstMapType > tensor_view( const Eigen::MatrixBase< Derived >& vec )
{
  return TensorView< typename Derived::ConstMapType >( Derived::Map(vec.derived().data(), vec.size()) ) ;
}

// mat * a == a ^ x
template <typename Derived>
void make_cross_mat( const Vec3& x, const Eigen::MapBase< Derived, Eigen::WriteAccessors >& map )
{
  Derived mat = map.derived() ;
  mat(0,0) =  0. ;
  mat(1,0) = -x[2] ;
  mat(2,0) =  x[1] ;
  mat(0,1) =  x[2] ;
  mat(1,1) =  0. ;
  mat(2,1) = -x[0] ;
  mat(0,2) = -x[1] ;
  mat(1,2) =  x[0] ;
  mat(2,2) =  0. ;
}

inline void make_cross_mat( const Vec3& x, Mat3& mat ) {
  return make_cross_mat( x, mat.block<3,3>(0,0) ) ;
}

void compute_anisotropy_matrix( const Mat3& A, Mat6 & Abar ) ;

} // mpm 

#endif


