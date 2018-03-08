


#ifndef mpm_field_func_h_
#define mpm_field_func_h_

#include "utils/alg.h"
#include "geometry.fwd.h"

namespace mpm{

template <typename Derived, ID D, typename MeshT>
struct FieldFuncBase
{
  typedef MeshBase<MeshT> MeshType;

  typedef typename Segmenter<D>::ValueType ValueType; 
  typedef typename Segmenter< D >::Seg Seg  ;

  explicit FieldFuncBase( const MeshType& mesh ) : m_mesh(mesh) {}

  ValueType operator[]( ID i ) const{
    ValueType v ;
    return eval_at_node( i, Segmenter< D >::val2seg(v) ) ;
  }

  void eval_at_node( ID i, Seg v ) const
  {
    return static_cast< const Derived& >(*this).eval_at_node(i, v) ;
  }


  ID size( ) const
  {
    return static_cast< const Derived& >(*this).size() ;
  }

  const MeshType& mesh() const { return m_mesh ; }



  protected:
    const MeshType& m_mesh ;

 }; 



} // namespace mpm


#endif