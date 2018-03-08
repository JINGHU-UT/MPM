
#include "tetGrid.h"

#include "boundaryInfo.h"

namespace mpm {

TetGridIterator& TetGridIterator::operator ++()
{
  ++cell[3] ;
  if(cell[3] == 6) {
    cell[3] = 0 ;
    ++cell[2] ;
    if(cell[2] == grid.dim()[2]) {
      cell[2] = 0 ;
      ++cell[1] ;
      if(cell[1] == grid.dim()[1]) {
        cell[1] = 0 ;
        ++cell[0] ;
      }
    }
  }

  return *this ;
}

ID TetGridIterator::index() const {
  return grid.cellID( cell ) ;
}

TetGrid::TetGrid(const Vec3 &box, const Vec3i &res)
  : Base()
{
  m_dim = res ;
  m_dx.array() = box.array()/res.array().cast< Scalar >() ;
  m_idx.array() = 1./m_dx.array() ;
}

void TetGrid::locate(const Vec3 &x, Location &loc) const
{
  Vec3 pos = x.array()*m_idx.array() ;

  loc.cell.segment<3>(0) =
      pos.cast< ID >().cwiseMax( Vec3i::Zero() ).cwiseMin(m_dim-Vec3i::Ones()) ;

//  pos -= loc.cell.segment<3>(0).cast< Scalar >() ;

  Tet geo ;
  for( loc.cell[3] = 0 ; loc.cell[3] < 6 ; ++loc.cell[3] )
  {
    get_geo(  loc.cell, geo ) ;
    geo.compute_coords( x, loc.coords );

    if( loc.coords.minCoeff() >= -1.e-12 )
      break ;
  }
}

void TetGrid::get_geo( const Cell &cell, CellGeo& geo ) const {
  const ID color = (cell[0]%2) + (cell[1]%2) * 2 + (cell[2]%2) * 4 ;


  get_corner( cell.segment<3>(0), geo.origin );
  geo.box = m_dx ;

  geo.update_geometry( color, cell[3] ) ;

}

void TetGrid::interpolate(const Location &loc, Interpolation &itp) const
{
  itp.coeffs = loc.coords ;

  Tet geo ;
  get_geo(  loc.cell, geo ) ;

  for( ID k = 0 ; k < NV ; ++k ) {
    const Vec3i v = ( geo.vertex(k).array() * m_idx.array()  + .5 ).cast<ID>() ;
    itp.nodes[k] = nodeID( v ) ;
  }
}

void TetGrid::get_derivatives( const Location& loc, Derivatives& dc_dx ) const
{
  Tet geo ;
  get_geo(  loc.cell, geo ) ;

  geo.compute_derivatives( loc.coords, dc_dx ) ;
}


void TetGrid::make_bc( const BoundaryMapper& mapper, BoundaryConditions &bc ) const
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
