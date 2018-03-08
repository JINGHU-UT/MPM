

#ifndef mpm_tet_h_
#define mpm_tet_h_

#include "utils/alg.h"


namespace mpm{

  struct Tet
  {
    static constexpr ID NV = 4 ;
    static constexpr ID NC = 4 ;
    static constexpr ID NQ = 4 ;

    typedef Eigen::Matrix<Scalar, NC, 1> Coords ; 
    typedef Eigen::Matrix<Scalar, NC, 3> Derivatives ; 

    typedef Eigen::Matrix<Scalar, NC, NQ> QuadPoints ; 
    typedef Eigen::Matrix<Scalar,  1, NQ> QuadWeights ; 

    typedef Eigen::Matrix< Scalar, 3, 4 > Vertices ;
    typedef Eigen::Matrix< Scalar, 3, Eigen::Dynamic > Points ;
    typedef Eigen::Matrix< Scalar, 6, Eigen::Dynamic > Frames ;


    struct {
      unsigned char part : 1 ; 
      unsigned char rot  : 3 ;
      unsigned char sym  : 3 ; 

      inline char rotate ( int d ) const { return rot&(1<<d) ; } // ?? 
      inline char sign   ( int d ) const {
        return 1 - 2*( ( sym&(1<<d) ) >> d )  ;  // ?? 
      }
    } geometry ; 

    enum Part {
      Left = 0 ,
      Right = 1 
    };

    Vec3 origin ; 
    Arr3 box   ; 

    int orientation ; 

    Tet()
      : geometry{0,0,0}
    {}


    Vec3 pos( const Coords& coords ) const {
  
      Vertices vtx  ;
      compute_vertices( vtx ) ;
  
      Vec3 p = vtx*coords ;
  
      to_world( p ) ;
      return p + origin ;
    }


    Vec3 center() const
    {
      return pos( Coords::Constant(1./NC) ) ;
    }

    Vec3 vertex( int cornerID ) const
    {
      Vec3 v ;
      offset( cornerID, v ) ;
  
      to_world(v) ;
      return v + origin ;
    }
  
    void vertexCoords( int cornerID, Coords& coords ) const {
      coords.setZero() ;
      coords[cornerID] = 1. ;
    }
  
  
    Scalar volume() const {
      return box.prod() / 6. ;
    }
  
    void compute_coords( const Vec3& pos, Coords & coords ) const {
      Vec3 local = pos - origin ;
      to_local( local );
      local_coords( local, coords );
    }
    void compute_derivatives( const Coords & coords, Derivatives& dc_dx ) const ;
  
    ID sample_uniform( const unsigned N, const ID start, Points &points, Frames &frames ) const ;
  
    void get_qp( QuadPoints& qps, QuadWeights& weights ) const {
      const Scalar a = (5. -   std::sqrt(5.) ) / 20 ;
      const Scalar b = (5. + 3*std::sqrt(5.) ) / 20 ;
  
      qps.setConstant( a ) ;
      qps.diagonal().setConstant( b ) ;
      weights.setConstant( volume() / NQ ) ;
    }
  
    void update_geometry( unsigned char rotation, int num ) ;

  private:
    void compute_vertices( Vertices& vertices ) const ;

    void offset( int cornerID, Vec3 &v ) const ;

    void to_world( Vec3& pos ) const ;
    void to_local( Vec3& pos ) const ;

    void local_coords( const Vec3& pos, Coords& coords ) const ;

  };




} // mpm



#endif