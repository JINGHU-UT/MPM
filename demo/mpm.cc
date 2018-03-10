
#include "utils/config.h"
#include "simulate/simulate.h"

#include "utils/log.h"
#include "utils/file.h"

#include <cstring>

namespace mpm {
	// extern const char* g_git_branch ;
	// extern const char* g_git_commit ;
	// extern const char* g_timestamp  ;
}

static void usage( const char *name )
{
	std::cout << "Usage: " << name
			  << " [sim_dir=out] [options] "
			  << "\nGranular material simulator. "
			  << "\n Output files are created inside the directory specified by `sim_dir`, which defaults to 'out'. "
			  << "\n\n" ;

	std::cout << "Options:\n"
			  << "-? \t Display this help message and exit\n"
			  << "-i file \t Load a configuration file  \n"
			  << "-v level \t Specify the verbosity level \n"
			  << "-key value \t Set the configuration parameter 'key' to 'value' ( see README.md )  \n"
			  << std::endl ;
}

int main( int argc, const char* argv[] )
{
	mpm::Config config ;

	const char * base_dir = "out" ;

	// Read coonfiguration from input files and CLI arguments
	for( int i = 1 ; i < argc ; ++i )
	{
		if( argv[i][0] == '-' ){
			if( std::strlen(argv[i]) > 2 ) {
				if( ++i == argc ) break ;
				config.from_string( argv[i-1]+1, argv[i] ) ;
			} else {
				switch(argv[i][1]) {
				case '?':
					usage( argv[0]) ;
					return 0;
				case 'i':
					if( ++i == argc ) break ;
					if( !config.from_file(argv[i]) ) {
						mpm::Log::Error() << "Error reading file " << argv[i] << std::endl ;
					}
					break ;
				case 'v':
					if( ++i == argc ) break ;
					mpm::Log::Config::get().setLevel( argv[i] ) ;
				}
			}
		} else {
			base_dir = argv[i] ;
		}
	}

	// std::string info = mpm::arg( mpm::arg3("%1 %3 on %2 [%4]", argv[0], mpm::g_git_branch, mpm::g_git_commit ), mpm::g_timestamp ) ;
	// mpm::Log::Info() << "This is " << info << std::endl ;

	// Save copy of final configuration and convert to interal units
	mpm::FileInfo outDir ( base_dir ) ;
	if( !outDir.exists() ) outDir.makeDir() ;
	// config.dump( outDir.filePath("config"), info.c_str() );
	config.internalize();

	mpm::Log::Debug() << "1/Re = " << config.viscosity << std::endl ;

	// Run simulation
	mpm::Simulate( config, base_dir ).run() ;

	return 0 ;
}
