


#ifndef mpm_mesh_base_h_
#define mpm_mesh_base_h_

#include "geometry.fwd.h"
#include "voxel.h"
#include "meshTraits.h"


namespace mpm{

  struct BoundaryInfo ; 
  struct BoundaryMapper ; 
  typedef std::vector<BoundaryInfo> BoundaryConditions ; 

  template <typename Derived>
  class MeshBase{
  public:

    typedef MeshTraits< Derived > Traits ;
    typedef typename Traits::CellIterator CellIterator ;
    typedef typename Traits::Cell    Cell  ;
    typedef typename Traits::Cells   Cells ;
    typedef typename Traits::CellGeo CellGeo ;

    enum {
      NV = Traits::NV ,
      NC = Traits::NC ,
      NQ = Traits::NQ
    } ;

    typedef Eigen::Matrix< Scalar, NC, 1 > Coords ;
    typedef Eigen::Matrix<  ID, NV, 1 > NodeList ;
    typedef Eigen::Matrix< Scalar, NV, 1 > CoefList ;
    typedef Eigen::Matrix< Scalar, NV, 3 > Derivatives ;


    struct Location {
      Cell      cell  ; // Cell
      Coords   coords ; // Coordinates in cell
    };
    
    struct Interpolation {
      NodeList nodes  ; // Adjacent node indices
      CoefList coeffs ; // Interpolation coeff for nodes
    };

    Derived& derived()
    { return static_cast< Derived& >( *this ) ; }
    const Derived& derived() const
    { return static_cast< const Derived& >( *this ) ; }

    ID nNodes() const { return derived().nNodes() ; }
    ID nCells() const { return derived().nCells() ; }

    ID cellID( const Cell& cell ) const
    { return derived().cellID( cell ) ; }

    Vec3 box() const { return derived().box() ; }
    Vec3 clamp_point( const Vec3& p ) const {
      return Vec3::Zero().cwiseMax(p).cwiseMin(box()) ;
    }

    void locate( const Vec3 &x, Location& loc ) const {
      derived().locate( x, loc ) ;
    }
    void interpolate( const Location& loc, Interpolation& itp ) const {
      derived().interpolate( loc, itp ) ;
    }
    void get_derivatives( const Location& loc, Derivatives& dc_dx ) const {
      derived().get_derivatives( loc, dc_dx ) ;
    }

    void interpolate( const Vec3 &x, Interpolation& itp ) const {
      Location loc ;
      derived().locate( x, loc ) ;
      derived().interpolate( loc, itp ) ;
    }
    void list_nodes( const Cell& cell, NodeList& list ) const {
      derived().list_nodes( cell, list ) ;
    }

    CellIterator cellBegin() const { return derived().cellBegin() ; }
    CellIterator cellEnd()  const { return derived().cellEnd() ; }

    void get_geo( const Cell &cell, CellGeo& geo ) const {
      derived().get_geo( cell, geo ) ;
    }

    void make_bc( const BoundaryMapper& mapper, BoundaryConditions &bc ) const {
      derived().make_bc( mapper, bc ) ;
    }

    ID nAdjacent( ID node ) const {
      return derived().nAdjacent( node ) ;
    }

    Vec3 pos( const Location& loc ) const ;


  };

} // mpm





#endif