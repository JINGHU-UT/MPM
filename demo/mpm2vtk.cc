
#include "utils/string.hh"

#include "visu/Offline.hh"
#include "visu/VTKParticlesWriter.hh"
#include "visu/VTKFieldWriter.hh"

#include "simulate/phase.h"

#include "geometry/scalarField.h"
#include "geometry/tensorField.h"

#include <iostream>

static void usage( const char *name )
{
	std::cout << "Usage: " << name
			  << " [sim_dir=out] [options] "
			  << "\nTransform raw simulation output into standard VTK files,"
			  << "\n sim_dir/vtk/fields-[frame_id].vtk and "
			  << "\n sim_dir/vtk/particles-[frame_id].vtk if the '-p' flag is provided"
			  << "\n\n" ;

	std::cout << "Options:\n"
			  << "-? \t Display this help message and exit\n"
			  << "-n frame_id \t Jump to frame frame_id\n"
			  << "-a \t Process all subsequent frames\n"
			  << "-p \t Create VTK file for particles as well \n"
			  << std::endl ;
}

void dump_frame( const mpm::Offline& offline, bool particles,
				 const char* base_dir, unsigned frame )
{

	if(particles) {
		mpm::VTKParticlesWriter particlesWriter( base_dir, offline.particles() ) ;
		particlesWriter.startFile( "particles", frame ) ;
		particlesWriter.dump_all() ;
	}

	mpm::VTKFieldWriter fieldWriter( base_dir, offline.mesh() ) ;
	//fieldWriter.setMode( mpm::VTKWriter::Ascii );
	fieldWriter.startFile( "fields", frame ) ;
	fieldWriter.dump(    "phi", offline.grains().fraction ) ;
	fieldWriter.dump(      "u", offline.grains().velocity ) ;
	fieldWriter.dump(  "d_phi", offline.grains().grad_phi ) ;
	fieldWriter.dump( "forces", offline.grains().fcontact ) ;

	mpm::ScalarField p   = offline.grains().stresses.trace() ;
	mpm::ScalarField dh  = offline.grains().sym_grad.trace() ;
	mpm::ScalarField taun= mpm::TensorField( offline.grains().stresses.deviatoricPart() ).norm() ;

	fieldWriter.dump(    "p", p ) ;
	fieldWriter.dump(   "dh", dh ) ;
	fieldWriter.dump( "taun", taun ) ;

	//	fieldWriter.dump( "lambda", offline.grains().stresses ) ;
}

int main( int argc, const char* argv[] ) {

	const char * base_dir = "out" ;
	unsigned frame = 0 ;

	bool all = false ;

	bool particles = false ;

	for( int i = 1 ; i < argc ; ++i )
	{
		if( argv[i][0] == '-' ){
			switch(argv[i][1]) {
			case '?':
				usage(argv[0]) ;
				return 0 ;
			case 'a':
				all = true ;
				break ;
			case 'p':
				particles = true ;
				break ;
			case 'n':
				if( ++i == argc ) break ;
				frame = mpm::to_uint( argv[i] ) ;
				break ;
			}
		} else {
			base_dir = argv[i] ;
		}
	}

	mpm::Offline offline( base_dir ) ;

	unsigned cur_frame = frame ;

	do {
		if(! offline.load_frame( cur_frame ) )
			return all?0:1 ;

		dump_frame( offline, particles, base_dir, cur_frame++ ) ;

	} while( all ) ;

	return (frame == cur_frame) ? 1 : 0 ;

}
