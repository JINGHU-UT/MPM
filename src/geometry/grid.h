

#ifndef mpm_grid_h_
#define mpm_grid_h_

#include "meshBase.h"
#include "voxel.h"

namespace mpm{

  class Grid ; 

  struct GridIterator
  {
    typedef MeshTraits< Grid >       Traits; 
    typedef typename Traits::Cell    Cell; 
    typedef typename Traits::CellGeo CellGeo ; 

    const Grid& grid; 
    Cell cell ; 

    GridIterator( Grid const& g, Cell const& c)
    : grid(g), cell(c)
    {}

    GridIterator& operator++() ;

    bool operator==( const GridIterator& o ) const
    {
      return cell == o.cell ;
    }
    bool operator!=( const GridIterator& o ) const
    {
      return cell != o.cell ;
    }

    Cell operator*() const {
      return cell ;
    }

    Index index() const ;

  };


  class Grid : public MeshBase< Grid >
  {
  public:
    typedef MeshBase< Grid > Base ;

    typedef typename Base::Cell    Cell    ;
    typedef typename Base::CellGeo CellGeo ;
    typedef          Vec3i         Vertex  ;

    Grid( const Vec3& box, const Vec3i &res ) ;
    
    void set_box( const Vec3& box ) ;

    Index nNodes() const
    { return (m_dim[0]+1) * (m_dim[1]+1) * (m_dim[2] + 1) ; }

    Index nCells() const
    { return (m_dim[0]) * (m_dim[1]) * (m_dim[2]) ; }

    Index cellIndex( const Cell& cell ) const
    {
      return (m_dim[2]) * (m_dim[1]) * cell[0]
        +  (m_dim[2]) * cell[1]
        +  cell[2] ;
    }

    Vec3 box() const
    { return firstCorner( m_dim ) ; }

    void locate( const Vec3 &x, Location& loc ) const ;

    using Base::interpolate ;
    void interpolate( const Location &loc , Interpolation& itp ) const ;

    void get_derivatives( const Location& loc, Derivatives& dc_dx ) const ;

    CellIterator cellBegin() const {
      return GridIterator( *this, Vec3i::Zero() ) ;
    }
    CellIterator cellEnd() const {
      return GridIterator( *this, Vec3i(m_dim[0],0,0) ) ;
    }

    void get_geo( const Cell &cell, CellGeo& geo ) const {
      get_corner( cell, geo.origin );
      geo.box = m_dx ;
    }

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

    Index nAdjacent( Index ) const {
      return NV ;
    }
    
    Vec3 nodePosition( const Vertex& node ) const
    {
      return firstCorner( node ) ;
    }

    Index nodeIndex( const Vertex& node ) const
    {
      return (m_dim[2]+1) * (m_dim[1]+1) * node[0]
        +  (m_dim[2]+1) * node[1]
        +  node[2] ;
    }
    
    const Vec3i& dim() const { return m_dim ; }

  private:

    const Vec3&    dx() const { return  m_dx ; }
    const Vec3&   idx() const { return m_idx ; }

    void get_corner( const Cell &cell, Vec3& corner ) const {
      corner = (cell.array().cast< Scalar >() * m_dx.array()).matrix() ;
    }

    void clamp_cell( Cell& cell ) const ;

    Vec3 firstCorner( const Cell &cell ) const
    { Vec3 corner ; get_corner( cell, corner ) ; return corner ; }

    Vec3i  m_dim ;
    Vec3   m_dx  ;
    Vec3   m_idx ;

    friend struct GridIterator ;
  } ;


} // mpm 




#endif