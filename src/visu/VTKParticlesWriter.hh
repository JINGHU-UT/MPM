
#ifndef MPM_VTK_PARTICLES_WRITER_HH
#define MPM_VTK_PARTICLES_WRITER_HH

#include "VTKWriter.hh"
#include "utils/alg.h"

namespace mpm {

class Particles ;

class VTKParticlesWriter : public VTKWriter
{

public:
	enum Quantity {
		Volumes,
		Velocities,
		Frames,
		Orientations
	} ;

	VTKParticlesWriter( const char* base_dir, const Particles& particles ) ;

	template< typename Derived >
	bool dump( const char* name, const Eigen::MatrixBase< Derived > &data ) ;

	bool dump( Quantity quantity ) ;
	bool dump_all( ) ;

protected:
	void writeMesh( File& file ) const ;
	size_t nDataPoints( ) const ;

private:

	const Particles& m_particles ;

};

} //mpm

#endif
