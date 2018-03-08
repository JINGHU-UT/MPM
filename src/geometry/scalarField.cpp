
#include "scalarField.h"
#include "fieldBase.impl.h"

#include "grid.h"
#include "tetGrid.h"

namespace mpm
{

template <typename MeshT >
Vec3 AbstractScalarField< MeshT >::grad_at( const Vec3& x ) const 
{
  typename MeshType::Location loc ;
  typename MeshType::NodeList nodes ;
  typename MeshType::Derivatives dc_dx ;

  m_mesh.locate( x, loc );
  m_mesh.list_nodes( loc.cell, nodes );
  m_mesh.get_derivatives( loc, dc_dx ) ;

  Vec3 grad = Vec3::Zero() ;
  for( Index k = 0 ; k < nodes.rows() ; ++k ) {
    grad += dc_dx.row(k) * Base::segment( nodes[k] ) ;
  }
  
  return grad ;
}

template class FieldBase< AbstractScalarField< Grid > > ;
template class AbstractScalarField< Grid > ;
template class FieldBase< AbstractScalarField< TetGrid > > ;
template class AbstractScalarField< TetGrid > ;

}
