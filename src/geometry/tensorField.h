


#ifndef MPM_TENSOR_FIELD_H_
#define MPM_TENSOR_FIELD_H_

#include "fieldBase.h"
#include "fieldFuncs.h"

namespace mpm {

template < typename MeshT >
struct FieldTraits< AbstractTensorField< MeshT > >  {
  typedef MeshT  MeshType ;
  typedef Vec6 ValueType ;
  static constexpr ID Dimension = 6 ;

};

template < typename MeshT >
class AbstractTensorField : public FieldBase< AbstractTensorField< MeshT > >
{
  typedef FieldTraits< AbstractTensorField > Traits ;
  typedef MeshBase< typename Traits::MeshType > MeshType ;

  typedef FieldBase< AbstractTensorField > Base ;

public:
  explicit AbstractTensorField( const MeshType& mesh )
    : Base( mesh )
  {
  }

  template <typename Func>
  AbstractTensorField( const FieldFuncBase< Func, Base::D, MeshT > & func )
    : Base( func.mesh() )
  {
    Base::operator=( func );
  }
  template <typename Func>
  AbstractTensorField& operator=( const FieldFuncBase< Func, Base::D, MeshT > & func )
  {
    return Base::operator=( func );
  }

  DeviatoricPart< MeshT > deviatoricPart() const {
    return DeviatoricPart< MeshT >( *this ) ;
  }
  FieldTrace< MeshT > trace() const {
    return FieldTrace< MeshT >( *this ) ;
  }
  FieldNorm< mpm::AbstractTensorField, MeshT > norm() const {
    return FieldNorm< mpm::AbstractTensorField, MeshT >( *this ) ;
  }

  void add_sym_tensor( const Vec3& x, Mat3& tensor ) const ;
  void get_sym_tensor( const Vec3& x, Mat3& tensor ) const ;

};


} //mpm

#endif


