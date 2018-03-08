
#ifndef mpm_VECTOR_FIELD_H_
#define mpm_VECTOR_FIELD_H_

#include "fieldBase.h"
#include "fieldFuncs.h"

#include "tensor.h"

namespace mpm {

template < typename MeshT >
struct FieldTraits< AbstractVectorField< MeshT > >  {

  typedef MeshT  MeshType ;
  typedef Vec3   ValueType ;
  static constexpr ID Dimension = 3 ;
};

template < typename MeshT >
class AbstractVectorField : public FieldBase< AbstractVectorField< MeshT > >
{
  typedef FieldTraits< AbstractVectorField > Traits ;
  typedef MeshBase< typename Traits::MeshType > MeshType ;

  typedef FieldBase< AbstractVectorField > Base ;

public:
  explicit AbstractVectorField( const MeshType& mesh )
    : Base( mesh )
  {

  }
  template <typename Func>
  AbstractVectorField( const FieldFuncBase< Func, Base::D, MeshT > & func )
    : Base( func.mesh() )
  {
    Base::operator=( func );
  }
  template <typename Func>
  AbstractVectorField& operator=( const FieldFuncBase< Func, Base::D, MeshT > & func )
  {
    return Base::operator=( func );
  }

  FieldNorm< mpm::AbstractVectorField, MeshT > norm() const {
    return FieldNorm< mpm::AbstractVectorField, MeshT >( *this ) ;
  }

  void get_spi_tensor( const Vec3& x, Mat3& tensor ) const ;
  void add_spi_tensor( const Vec3& x, Mat3& tensor ) const ;

};


} //mpm

#endif

