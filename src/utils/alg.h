#ifndef mpm_util_alg_h
#define mpm_util_alg_h


#include "scalar.h"
#include <Eigen/Core>

namespace mpm{

  typedef Eigen::Matrix<Scalar, Eigen::Dynamic, 1> DynVec;
  typedef Eigen::Array<Scalar, Eigen::Dynamic, 1> DynArr;
  typedef Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> DynMat;

  typedef Eigen::Matrix<Scalar, 3, 3 > Mat3 ; 
  typedef Eigen::Matrix<Scalar, 3, 1 > Vec3 ; 
  typedef Eigen::Array<Scalar, 3, 1 > Arr3 ; 

  typedef Eigen::Matrix<ID, 3, 1 > Vec3i ;
  typedef Eigen::Array<ID, 3, 1 > Arr3i ; 

  typedef Eigen::Matrix<Scalar, 6, 6 > Mat6 ;
  typedef Eigen::Matrix<Scalar, 6, 1 > Vec6 ; 

  typedef Eigen::Matrix<Scalar, 3, Eigen::Dynamic> DynMat3 ; 
  typedef Eigen::Matrix<Scalar, 6, Eigen::Dynamic> DynMat6 ; 

  template <ID Dimension>
  struct Segmenter{
    typedef Eigen::Matrix<Scalar, Dimension, 1 > ValueType ;
    typedef typename DynVec::template FixedSegmentReturnType<Dimension>::Type Seg ; 
    typedef typename DynVec::template ConstFixedSegmentReturnType<Dimension>::Type ConstSeg;

    static inline Seg segment(DynVec& vec, const ID i){
      return vec.template segment< Dimension > ( i*Dimension );
    }
    static inline ConstSeg segment( const DynVec& vec, const ID i){
      return vec.template segment< Dimension > ( i*Dimension );
    }
    Seg val2seg( ValueType & v ){ return v.segment<Dimension>(0) ; }
  };

  template < > 
  struct Segmenter<1> {
    typedef Scalar ValueType ;

    typedef Scalar& Seg ; 
    typedef const Scalar& ConstSeg ; 

    static inline Seg segment(DynVec& vec, const ID i ) {
      return vec[ i ] ; 
    }
    static inline ConstSeg segment( const DynVec& vec, const ID i){
      return vec[ i ] ; 
    }
    Seg val2seg(ValueType& v) { return v ;}
  };

  template < typename Derived >
  inline void set_zero( Eigen::MatrixBase< Derived >& mat ) {
    mat.setZero() ;
  }
  inline void set_zero( Scalar& s ) {
    s = 0 ;
  }

  template < ID Dimension, typename Derived >
  void mul_compwise( DynVec& vec, const Eigen::MatrixBase<Derived>& mat ) {
    assert( vec.rows() == mat.rows() * Dimension ) ;
    Eigen::Matrix< Scalar, Dimension, Eigen::Dynamic >::Map( vec.data(), Dimension, mat.rows() )
        *= mat.asDiagonal() ;
  }
  template < ID Dimension, typename Derived >
  void div_compwise( DynVec& vec,  const Eigen::MatrixBase<Derived> & mat ) {
    assert( vec.rows() == mat.rows() * Dimension ) ;
    Eigen::Matrix< Scalar, Dimension, Eigen::Dynamic >::Map( vec.data(), Dimension, mat.rows() )
        *= (1./mat.array()).matrix().asDiagonal() ;
  }
  template < ID Dimension, typename Derived >
  void set_compwise( DynVec& vec, const Eigen::MatrixBase< Derived > & mat ) {
    vec.setOnes( Dimension * mat.rows() ) ;
    mul_compwise< Dimension >( vec, mat ) ;
  }

  template < ID Dimension >
  typename Eigen::Matrix< Scalar, Dimension, Eigen::Dynamic >::MapType::RowXpr component( DynVec& vec, ID row ) {
     return Eigen::Matrix< Scalar, Dimension, Eigen::Dynamic >::Map( vec.data(), Dimension, vec.rows()/Dimension ).row( row ) ;
   }

  template < ID Dimension >
  typename Eigen::Matrix< Scalar, Dimension, Eigen::Dynamic >::ConstMapType::ConstRowXpr component( const DynVec& vec, ID row ) {
     return Eigen::Matrix< Scalar, Dimension, Eigen::Dynamic >::Map( vec.data(), Dimension, vec.rows()/Dimension ).row( row ) ;
   }

}





#endif