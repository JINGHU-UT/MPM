
#ifndef MPM_PHASE_DESCR_H_
#define MPM_PHASE_DESCR_H_

#include "geometry/geometry.fwd.h"
#include "geometry/scalarField.h"
#include "geometry/vectorField.h"
#include "geometry/tensorField.h"

#include "geometry/mesh.impl.h"

namespace mpm {

struct Phase
{

	ScalarField fraction ;
	VectorField velocity ;

	TensorField stresses ;
	TensorField sym_grad ;
	VectorField spi_grad ;

	VectorField grad_phi ;
	VectorField geo_proj ;
	VectorField fcontact ;

	Phase( const MeshType& mesh )
		: fraction(mesh), velocity(mesh),
		  stresses(mesh), sym_grad(mesh),
		  spi_grad(mesh), grad_phi(mesh),
		  geo_proj(mesh), fcontact(mesh)
	{}

	template < typename Archive >
	void serialize( Archive &ar, unsigned int ) {
		ar & fraction ;
		ar & velocity ;
		ar & stresses ;
		ar & sym_grad ;
		ar & spi_grad ;
		ar & grad_phi ;
		ar & fcontact ;
	}
};

} //mpm

#endif
