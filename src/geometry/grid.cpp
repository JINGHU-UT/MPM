


#include "grid.h"
#include "boundaryInfo.h"

namespace mpm{

  GridIterator& GridIterator::operator ++()
  {
    ++cell[2] ;
    if(cell[2] == grid.dim()[2]) {
      cell[2] = 0 ;
      ++cell[1] ;
      if(cell[1] == grid.dim()[1]) {
        cell[1] = 0 ;
        ++cell[0] ;
      }
    }

    return *this ;
  }

  ID GridIterator::index() const {
    return grid.cellID( cell ) ;
  }


  Grid::Grid(const Vec3 &box, const Vec3i &res)
    : Base()
  {
    m_dim = res ;
    set_box( box ) ;
  }

  void Grid::set_box( const Vec3& box )
  {
    m_dx.array() = box.array()/m_dim.array().cast< Scalar >() ;
    m_idx.array() = 1./m_dx.array() ;
  }

  void Grid::clamp_cell( Cell & cell ) const
  {
    cell = Vec3i::Zero().cwiseMax(cell).cwiseMin(m_dim-Vec3i::Ones()) ;
  }

  void Grid::locate(const Vec3 &x, Location &loc) const
  {
    loc.coords = x.array()*m_idx.array() ;
    loc.cell = loc.coords.cast< ID >();
    clamp_cell( loc.cell) ;

    loc.coords -= loc.cell.cast< Scalar >() ;
  }

  void Grid::interpolate(const Location &loc, Interpolation &itp) const
  {
    for( int i = 0 ; i < 2 ; ++i )
      for( int j = 0 ; j < 2 ; ++j )
        for( int k = 0 ; k < 2 ; ++k ) {
          const Cell corner (i,j,k) ;
          const int idx = Voxel::cornerID( i, j, k ) ;
          itp.nodes[ idx ] = nodeID( loc.cell + corner ) ;
          itp.coeffs[ idx ] = Voxel::cornerCoeff( corner, loc.coords );
        }

  }

  void Grid::get_derivatives( const Location& loc, Derivatives& dc_dx ) const
  {
    for( int i = 0 ; i < 2 ; ++i )
      for( int j = 0 ; j < 2 ; ++j )
        for( int k = 0 ; k < 2 ; ++k ) {
          const Cell corner (i,j,k) ;
          const int idx = Voxel::cornerID( i, j, k ) ;
          Voxel::getCornerDerivatives( corner, loc.coords, dc_dx.row( idx ) );
        }



    for (int k = 0 ; k < 3 ; ++k){
      dc_dx.col( k ) *= m_idx[k] ;
    }

// std::cerr << " Grid::get_derivatives " << dc_dx << std::endl;

  }


  void Grid::make_bc( const BoundaryMapper& mapper, BoundaryConditions &bc ) const
  {
    for( ID i = 0 ; i <= m_dim[1] ; ++i ) {
      for( ID j = 0 ; j <= m_dim[2] ; ++j ) {
        bc[ nodeID( Vertex(0       , i, j) ) ].combine( mapper( "left"), Vec3(-1,0,0) ) ;
        bc[ nodeID( Vertex(m_dim[0], i, j) ) ].combine( mapper("right"), Vec3( 1,0,0) ) ;
      }
    }
    for( ID i = 0 ; i <= m_dim[0] ; ++i ) {
      for( ID j = 0 ; j <= m_dim[2] ; ++j ) {
        bc[ nodeID( Vertex(i, 0       , j) ) ].combine( mapper("front"), Vec3(0,-1,0) ) ;
        bc[ nodeID( Vertex(i, m_dim[1], j) ) ].combine( mapper( "back"), Vec3(0, 1,0) ) ;
      }
    }
    for( ID i = 0 ; i <= m_dim[0] ; ++i ) {
      for( ID j = 0 ; j <= m_dim[1] ; ++j ) {
        bc[ nodeID( Vertex(i, j, 0       ) ) ].combine( mapper("bottom"), Vec3(0,0,-1) ) ;
        bc[ nodeID( Vertex(i, j, m_dim[2]) ) ].combine( mapper(   "top"), Vec3(0,0, 1) ) ;
      }
    }
  }



}