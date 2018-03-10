
#ifndef mpm_VOXEL_HH
#define mpm_VOXEL_HH

#include "utils/alg.h"
#include "utils/scalar.h"
#include <iostream>

namespace mpm {

struct Voxel {

  static constexpr ID NV = 8 ;
  static constexpr ID NC = 3 ;
  static constexpr ID NQ = 8 ;

  typedef Eigen::Matrix< Scalar, NC, 1 > Coords ;
  typedef Eigen::Matrix< Scalar, NC, NQ> QuadPoints ;
  typedef Eigen::Matrix< Scalar,  1, NQ> QuadWeights ;
  typedef Eigen::Matrix< Scalar, 3, Eigen::Dynamic > Points ;
  typedef Eigen::Matrix< Scalar, 6, Eigen::Dynamic > Frames ;

  Vec3 origin ;  //!< 3D coords of first corner
  Vec3 box    ;  //!< Dimensions of cell

  template < typename Idx >
  static inline Idx cornerID( Idx i, Idx j, Idx k ) {
    return (i << 0) + (j << 1) + (k << 2) ;
  }

  static inline ID cornerID( const Vec3i& corner ) {
    return cornerID< ID >( corner[0], corner[1], corner[2] ) ;
  }

  void vertexCoords( int cornerID, Coords& coords ) const {
    coords = Vec3( (cornerID&1)>>0, (cornerID&2)>>1, (cornerID&4)>>2 ) ;
  }


  static inline Scalar cornerCoeff( const Vec3i& corner, const Coords &coords ) {
    // c_i(x) = i + (1 - 2*i )*( 1- x) = [ 1-x if i=0, 1 + (-1)(1-x) = x if i = 1 ]
    return ( corner.cast< Scalar >().array() + ( Coords::Ones() - 2*corner.cast< Scalar >() ).array()
        * ( Coords::Ones() - coords ).array() ).prod() ;
  }

  // \warning lacs scaling by 1./box
  template < typename Res >
  static void getCornerDerivatives( const Vec3i& corner, const Coords &coords, Res res ) {
    const Vec3 coeffs =  ( corner.cast< Scalar >().array() + ( Coords::Ones() - 2*corner.cast< Scalar >() ).array()
        * ( Coords::Ones() - coords ).array() ) ;
    Vec3 copy ;
    for( int k = 0 ; k < 3 ; ++k ) {
      // d (c^k_i(x)) /dx _k = (2 * i - 1)
      copy = coeffs ; copy[k] = 2*corner[k] - 1 ;
      res[k] = copy.prod() ;
    }

    // std::cerr << "getCornerDerivatives res = " << res << std::endl; 
  }

  Vec3 center() const {
    return origin + .5*box ;
  }

  Vec3 pos( const Coords& coords ) const {
    return origin.array() + coords.array()*box.array() ;
  }


  Vec3 vertex( int cornerID ) const {
    Coords coords ;
    vertexCoords( cornerID, coords );
    return origin + ( coords.array() * box.array() ).matrix() ;
  }



  Scalar volume() const { return box.prod() ; }

  ID sample_uniform( const unsigned N, const ID start, Points &points, Frames &frames ) const ;

  static QuadPoints Qps()  ;

  void get_qp( QuadPoints& qp, QuadWeights& weights ) const {
    static const QuadPoints s_qps = Qps() ;
    qp = s_qps ;
    weights.setConstant( volume() / NQ ) ;
  }

} ;



} //end Voxel

#endif


