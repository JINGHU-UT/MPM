
#ifndef MPM_SCALAR_FIELD_H_
#define MPM_SCALAR_FIELD_H_

#include "fieldBase.h"

namespace mpm {

template < typename MeshT >
struct FieldTraits< AbstractScalarField< MeshT > >  {
  typedef MeshT  MeshType ;
  typedef Scalar ValueType ;
  static constexpr ID Dimension = 1 ;
};

template < typename MeshT >
class AbstractScalarField : public FieldBase< AbstractScalarField< MeshT > >
{
  typedef FieldTraits< AbstractScalarField > Traits ;
  typedef MeshBase< typename Traits::MeshType > MeshType ;

  typedef FieldBase< AbstractScalarField > Base ;

public:
  explicit AbstractScalarField( const MeshType& mesh )
    : Base( mesh )
  {

  }

  // Naive local gradient
  Vec3 grad_at( const Vec3& x) const ;

  template <typename Func>
  AbstractScalarField( const FieldFuncBase< Func, Base::D, MeshT > & func )
    : Base( func.mesh() )
  {
    Base::operator=( func );
  }
  template <typename Func>
  AbstractScalarField& operator= ( const FieldFuncBase< Func, Base::D, MeshT > & func )
  {
    return Base::operator=( func );
  }

protected:
  using Base::m_mesh ;
};


} //mpm

#endif
