

#ifndef mpm_tetgrid_h_
#define mpm_tetgrid_h_

#include "meshBase.h"
#include "tet.h"

namespace mpm{

  class TetGrid;
  struct TetGridIterator
  {
    typedef MeshTraits< TetGrid > Traits ;
    typedef typename Traits::Cell    Cell ;
    typedef typename Traits::CellGeo CellGeo ;

    const TetGrid& grid ;
    Cell cell ;

    TetGridIterator( const TetGrid& g, const Cell& c )
      : grid(g), cell(c)
    {}

    TetGridIterator& operator++() ;

    bool operator==( const TetGridIterator& o ) const
    {
      return cell.isApprox( o.cell );
    }
    bool operator!=( const TetGridIterator& o ) const
    {
      return ! cell.isApprox( o.cell );
    }

    Cell operator*() const {
      return cell ;
    }

    Index index() const ;

  };





  class TetGrid : public MeshBase< TetGrid >
  {
  public:
    typedef MeshBase< TetGrid > Base ;

    typedef typename Base::Cell Cell ;
    typedef typename Base::CellGeo CellGeo ;
    typedef Vec3i         Vertex ;

    TetGrid( const Vec3& box, const Vec3i &res ) ;


    Index nNodes() const
    { return (m_dim[0]+1) * (m_dim[1]+1) * (m_dim[2] + 1) ; }

    Index nCells() const
    { return 6 * (m_dim[0]) * (m_dim[1]) * (m_dim[2]) ; }

    Index cellIndex( const Cell& cell ) const
    {
      return  (  (m_dim[2]) * (m_dim[1]) * cell[0]
          +  (m_dim[2]) * cell[1]
          +  cell[2] ) * 6 + cell[3] ;
    }

    Vec3 box() const
    { return firstCorner( m_dim ) ; }

    void locate( const Vec3 &x, Location& loc ) const ;

    using Base::interpolate ;
    void interpolate( const Location &loc , Interpolation& itp ) const ;

    void get_derivatives( const Location& loc, Derivatives& dc_dx ) const ;

    CellIterator cellBegin() const {
      return TetGridIterator( *this, Cell::Zero() ) ;
    }
    CellIterator cellEnd() const {
      return TetGridIterator( *this, Cell(m_dim[0],0,0,0) ) ;
    }

    void get_geo( const Cell &cell, CellGeo& geo ) const  ;

    template < typename Archive >
    void serialize( Archive &ar, unsigned int ) {
      ar & m_dim ;
      ar &  m_dx ;
      ar & m_idx ;
    }

    void list_nodes( const Cell& cell, NodeList& list ) const {
      Location loc { cell, Coords::Zero() } ;
      Interpolation itp ;
      interpolate( loc, itp );
      list = itp.nodes ;
    }

    void make_bc( const BoundaryMapper& mapper, BoundaryConditions &bc ) const ;
    
    Index nAdjacent( Index idx ) const {
      Vec3i node ;
      node[0] = idx / ( (m_dim[2]+1) * (m_dim[1]+1) ) ;
      idx -= node[0] * (m_dim[2]+1) * (m_dim[1]+1) ;
      node[1] = idx / (m_dim[2]+1) ;
      node[2] = idx - node[1]*(m_dim[2]+1)  ;

      // (0,0,1) et (1m,0,0)
      return ( ( (node[0]%2) == (node[1]%2) ) && ( (node[0]%2) != (node[2]%2) ) )
        ? 48 : 16 ;

    }

  private:

    const Vec3i& dim() const { return m_dim ; }
    const Vec3&    dx() const { return  m_dx ; }
    const Vec3&   idx() const { return m_idx ; }



    Index nodeIndex( const Vertex& node ) const
    {
      return (m_dim[2]+1) * (m_dim[1]+1) * node[0]
        +  (m_dim[2]+1) * node[1]
        +  node[2] ;
    }

    void get_corner( const Vec3i &cell, Vec3& corner ) const {
      corner = (cell.array().cast< Scalar >() * m_dx.array()).matrix() ;
    }

    Vec3 firstCorner( const Vec3i &cell ) const
    { Vec3 corner ; get_corner( cell, corner ) ; return corner ; }

    Vec3i m_dim ;
    Vec3   m_dx  ;
    Vec3   m_idx  ;

    friend struct TetGridIterator ;
  };
}

#endif