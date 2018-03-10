

#ifndef MPM_ACTIVE_INDICES_H_
#define MPM_ACTIVE_INDICES_H_

#include "geometry/scalarField.h"
#include "geometry/vectorField.h"
#include "geometry/tensorField.h"

#include "geometry/mesh.impl.h"

namespace mpm{

struct Active
{
  static const ID s_Inactive ; 

  ID nNodes ; //!< Number of active nodes
  ID offset ; //!< Offset in global simulation nodes array
  typename MeshType::Cells cells ;  //!< List of active cells

  std::vector< ID > indices    ; //!< mesh_index -> simu_index
  std::vector< ID > revIndices ; //!< simu_index -> mesh_index

  Active()   : nNodes( 0 ), offset(0) {}

  void reset( ID totNodes )
  {
    offset = 0 ;
    nNodes = 0 ;
    cells.clear();
    revIndices.clear() ;
    indices.assign( totNodes, s_Inactive );
  }

  void computeRevIndices() ;
  void setOffset( const ID o ) ;

  ID count() const { return nNodes ; }
  ID origSize() const { return indices.size() ; }

  //! Conversion from field on whole mesh to values at active nodes
  template < typename Derived >
  void field2var( const FieldBase<Derived> &field, DynVec & var, bool resize = true ) const ;
  
  //! Conversion from values at active nodes to field on whole mesh (zero at missing nodes)
  template < typename Derived >
  void var2field( const DynVec & var, FieldBase<Derived> &field ) const ;

};

}





#endif