
#ifndef mpm_FORM_BUILDER_H_
#define mpm_FORM_BUILDER_H_

#include "geometry/geometry.fwd.h"
#include "geometry/meshBase.h"

#include "utils/block_mat.h"

#include "geometry/mesh.impl.h"
#include "geometry/tensor.h"

#include <bogus/Core/Block.impl.hpp>
#include <algorithm>

namespace mpm {

class Particles ;

//! Utility class for building matrices of bilinear forms
class FormBuilder {

	typedef typename FormMat< 3,3 >::Type::MajorIndexType          CompressedIndexType ;
	typedef typename CompressedIndexType::Index BgIndex ;

	typedef const std::vector< ID > &Indices ;
	typedef const typename MeshType::Interpolation& Itp ;
	typedef const typename MeshType::Derivatives& Dcdx ;


public:

	FormBuilder( const MeshType& mesh )
		: m_mesh(mesh)
	{}

	void reset( ID rows ) ;
	ID rows() const { return m_data.size() ; }

	//! Computes matrices non-zero blocks (nodes sharing a cell) from list of active cells
	void addToID(
			const typename MeshType::Cells& cells,
			Indices rowIndices, Indices colIndices	 ) ;

	void addRows( ID rows ) ;

	//! Compute compressed sparse block matrix index
	void makeCompressed() ;


	//! Integrate over quadrature points
	template < typename Func >
	void integrate_qp( const typename MeshType::Cells& cells, Func func	) const ;
	template < typename CellIterator, typename Func >
	void integrate_qp( const CellIterator& cellBegin, const CellIterator& cellEnd, Func func ) const ;

	//! Integrate over nodes ( trapezoidal approx )
	template < typename Func >
	void integrate_node( const typename MeshType::Cells& cells, Func func	) const ;
	template < typename CellIterator, typename Func >
	void integrate_node( const CellIterator& cellBegin, const CellIterator& cellEnd, Func func ) const ;

	//! Integrate over particles (MPM)
	template < typename Func >
	void integrate_particle( const Particles& particles, Func func ) const  ;

	// Building blocks

	static void addDuDv     ( FormMat<3,3>::Type& A, Scalar w,
							  ID rowIndex, const typename MeshType::Derivatives::ConstRowXpr& row_dx,
							   Itp itp, Dcdx dc_dx, Indices colIndices ) ;

	static void addVDp      ( FormMat<3,1>::Type& A, Scalar w, ID rowIndex, Itp itp, Dcdx dc_dx, Indices colIndices ) ;
	static void addTauDu    ( FormMat<6,3>::Type& A, Scalar w, ID rowIndex, Itp itp, Dcdx dc_dx, Indices colIndices ) ;
	static void addTauWu    ( FormMat<3,3>::Type& A, Scalar w, ID rowIndex, Itp itp, Dcdx dc_dx, Indices colIndices ) ;

	static void addDuDv     ( FormMat<3,3>::Type& A, Scalar w, Itp itp, Dcdx dc_dx, Indices rowIndices, Indices colIndices ) ;
	static void addVDp      ( FormMat<3,1>::Type& A, Scalar w, Itp itp, Dcdx dc_dx, Indices rowIndices, Indices colIndices ) ;
	static void addTauDu    ( FormMat<6,3>::Type& A, Scalar w, Itp itp, Dcdx dc_dx, Indices rowIndices, Indices colIndices ) ;
	static void addTauWu    ( FormMat<3,3>::Type& A, Scalar w, Itp itp, Dcdx dc_dx, Indices rowIndices, Indices colIndices ) ;

	static void addUTaunGphi( FormMat<6,3>::Type& A, Scalar w, Itp itp, const Vec3& dphi_dx, Indices rowIndices, Indices colIndices ) ;
	static void addUTauGphi ( FormMat<6,3>::Type& A, Scalar w, Itp itp, const Vec3& dphi_dx, Indices rowIndices, Indices colIndices ) ;

	const CompressedIndexType& index() { return m_compressed ; }

private:

	const MeshType &m_mesh ;

	CompressedIndexType m_compressed ;

	std::vector< std::vector< BgIndex > > m_data ;

};


} //mpm

#endif
