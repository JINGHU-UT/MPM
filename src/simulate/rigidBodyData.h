#ifndef mpm_RIGID_BODY_DATA_H_
#define mpm_RIGID_BODY_DATA_H_

// #include "utils/block_mat.h"

#include "activeIndices.h"
#include "rigidBody.h"

#include "formBuilder.h"
#include "formBuilder.impl.h"

#include "geometry/levelSet.h"
#include "geometry/boundaryInfo.h"
#include "geometry/tensor.h"
#include "geometry/tensorField.h"
#include "geometry/mesh.impl.h"
#include "geometry/meshCell.h"

namespace mpm {

class RigidBody ;

struct RigidBodyData
{
	RigidBodyData( RigidBody& rb_, TensorField &s ) ;

	//! Volume fraction taken by the rigid-body at a given point
	Scalar    phi( const Vec3 &x ) const ;
	//! Gradient of the volume fraction taken by the rigid-body at a given point
	void grad_phi( const Vec3 &x, Vec3 &grad ) const ;

	//! Computes nodes that are influenced by the rigid-body
	void compute_active( const Active& phaseNodes, BoundaryConditions &bc ) ;
	//! Assembles projection and jacobian matrices
	void assemble_matrices( const Active& phaseNodes, ID totNodes ) ;

	RigidBody&   rb ;
	TensorField& stresses ;

	Active	    nodes ;
	typename MeshType::Cells occupiedCells ;

	FormMat<6,3>::Type	jacobian ;     //!< int( (u grad phi):tau )
	FormMat<6,3>::Type	projection ;   //!< Linear operator giving rb velocities at mesh nodes

	DynVec fraction ;  //!< Interpolated volume fraction at occupied nodes

private:
	static const Scalar s_splatRad ;

	void integrate(const Active& phaseNodes, ID totNodes  ) ;

};

} //mpm

#endif
