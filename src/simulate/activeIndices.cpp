
#include "activeIndices.h"



namespace mpm {

const ID Active::s_Inactive = -1 ;

void Active::computeRevIndices()
{
	revIndices.resize( nNodes );

#pragma omp parallel for
	for( size_t i = 0 ; i < indices.size() ; ++i  ) {
		const ID idx = indices[ i ] ;
		if( idx != Active::s_Inactive ) {
			revIndices[idx-offset] = i ;
		}
	}
}

void Active::setOffset(const ID dist)
{
	offset = dist ;

#pragma omp parallel for
	for( size_t i = 0 ; i < indices.size() ; ++i  ) {
		if( indices[i] != Active::s_Inactive )
			indices[i] += dist ;
	}
}

template < typename Derived >
void Active::field2var( const FieldBase<Derived> &field, DynVec& var, bool resize ) const
{
	constexpr ID D = FieldBase<Derived>::D ;

	if( resize )
		var.resize( (offset + D) * count() );

#pragma omp parallel for
	for( ID i = 0 ; i < nNodes ; ++ i) {
		const ID idx = revIndices[ i ] ;
		Segmenter<D>::segment( var, offset+i ) = field[ idx ] ;
	}
}

template < typename Derived >
void Active::var2field( const DynVec& var,  FieldBase<Derived> &field ) const
{
	constexpr ID D = FieldBase<Derived>::D ;

	field.set_zero();

#pragma omp parallel for
	for( ID i = 0 ; i < nNodes ; ++ i) {
		const ID idx = revIndices[ i ] ;
		field[ idx ] = Segmenter<D>::segment( var, offset+i ) ;
	}
}


template void Active::var2field( const DynVec & var, FieldBase<ScalarField> &field ) const;
template void Active::var2field( const DynVec & var, FieldBase<VectorField> &field ) const;
template void Active::var2field( const DynVec & var, FieldBase<TensorField> &field ) const;
template void Active::field2var( const FieldBase<ScalarField> &field, DynVec & var, bool ) const;
template void Active::field2var( const FieldBase<VectorField> &field, DynVec & var, bool ) const;
template void Active::field2var( const FieldBase<TensorField> &field, DynVec & var, bool ) const;

} //mpm
