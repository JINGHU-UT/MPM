
#ifndef mpm_FIELD_BASE_IMPL_HH
#define mpm_FIELD_BASE_IMPL_HH

#include "FieldBase.h"

#include "MeshBase.h"
#include "ScalarField.h"

namespace mpm {
  
  template< typename Derived >
  void FieldBase< Derived >::eval_at( const Vec3& x, ValueType& res ) const
  {
    typename MeshType::Interpolation itp ;
    m_mesh.interpolate( x, itp );
  
    mpm::set_zero( res ) ;
    for( Index k = 0 ; k < itp.nodes.rows() ; ++k ) {
      res += itp.coeffs[k] * segment( itp.nodes[k] ) ;
    }
  
  }
  
  template< typename Derived >
  void FieldBase< Derived >::add_at( const Vec3& x, const ValueType& val )
  {
    typename MeshType::Interpolation itp ;
    m_mesh.interpolate( x, itp );
    add_at( itp, val ) ;
  }
  
  template< typename Derived >
  void FieldBase< Derived >::add_at( const typename MeshType::Interpolation &itp, const ValueType&   val )
  {
    for( Index k = 0 ; k < itp.nodes.rows() ; ++k ) {
      segment( itp.nodes[k] ) += itp.coeffs[k] * val ;
    }
  }
  

  template< typename Derived >
  Derived& FieldBase< Derived >::set_zero() {
    m_data.setZero() ;
    return derived() ;
  }
  

  template< typename Derived >
  Derived& FieldBase< Derived >::set_constant(const ValueType &val) {
  #pragma omp parallel for
    for( Index i = 0 ; i < m_size ; ++i ) {
      segment(i) = val ;
    }
    return derived() ;
  }

  
  template< typename Derived >
  Derived& FieldBase< Derived >::multiply_by(const ScalarField &field) {
    mul_compwise< D >( m_data, field.flatten() ) ;
    return derived() ;
  }

  
  template< typename Derived >
  Derived& FieldBase< Derived >::divide_by(const ScalarField &field) {
    div_compwise< D >( m_data, field.flatten() ) ;
    return derived() ;
  }

  
  template< typename Derived >
  Derived& FieldBase< Derived >::divide_by_positive(const ScalarField &field, Scalar min) {
    div_compwise< D >( m_data, field.flatten().cwiseMax(min) ) ;
    return derived() ;
  }

} // namespace mpm

#endif
