

#include "config.h"

namespace mpm{
  template<typename dest_t>
  void rescale( dest_t&  ,  Scalar  ){ }

  template<>
  void rescale( Scalar& src, Scalar s){ src *= s ; }

  template<int Dim>
  void rescale( Eigen::Matrix<Scalar, Dim, 1>& src, Scalar s ){ src *= s; }

  Config::Config()
  : fps(240), substeps(1), nFrames( 1 ),
    box(1,1,1), res(10,10,10),
    nSamples(2), randomize( 0 ),
    volMass( 1.5e3 ),
    viscosity( 1.e-3 ),
    gravity( 0, 0, -9.81 ),
    phiMax(1), mu(0),
    delta_mu( 0 ), I0( 0.4 ), grainDiameter( 1.e-3 ),
    muRigid( 0.5 ),
    cohesion(0), cohesion_decay(0),
    anisotropy( 0 ), elongation( 1 ), brownian( 0 ),
    initialOri( 1./3, 1./3, 1./3 ),
    enforceMaxFrac( false ), weakStressBC( false ),
    boundary("cuve"),
    output( true ), dumpPrimalData( 0 )
  {}


  void Config::internalize()
  {
    m_units.setTypical( box.minCoeff()/res.maxCoeff(), gravity.norm(), volMass ) ; 
    #define CONFIG_FIELD( name, type, u ) \
      rescale( name, m_units.fromSI( u ) ) ;
      EXPAND_CONFIG
    #undef CONFIG_FIELD
  }


  bool Config::from_string( std::string const& key, std::string const& val ){
    std::istringstream iss( val );
    return from_string( key, iss ) ; 
  }

  bool Config::from_string( std::string const& key, std::istringstream& iss ){
    bool f = false; 
    #define CONFIG_FIELD( name, type, s ) \
      if(key == mpm_stringify(name)) { cast(iss, name) ; f = true ; }
      EXPAND_CONFIG
    #undef CONFIG_FIELD

    if( !f ) Log::Warning() << "Warning: '" << key << "' is not a valid config field" << std::endl;

    return f ;
  }


  bool Config::from_file(const std::string &file_name)
  {
    std::ifstream ifs( file_name ) ;
    if(!ifs){
      Log::Warning() << "Warning: Config::from_file() file_name [";
      Log::Warning() << file_name << "] is not valid" << std::endl;
      return false ;
    }

    std::string line, key ;

    while( std::getline( ifs, line ))
    {
      std::istringstream iss ( line ) ;
      if ( (iss >> key) && key[0] != '#' )
      {
        from_string( key, iss ) ;
      }
    }

    return true ;
  }


  bool Config::dump(const std::string& file_name, const char *comment ) const
  {
    std::ofstream ofs( file_name ) ;
    if(!ofs) {
      Log::Error() << "Could not write config file " << file_name << std::endl ;
      return false ;
    }

    if(comment) {
      ofs << "# " << comment << std::endl ;
    }

    #define CONFIG_FIELD( name, type, s ) \
      ofs << mpm_stringify(name) << "\t" ; mpm::dump( ofs, name ) ; ofs << "\n" ;
      EXPAND_CONFIG
    #undef CONFIG_FIELD

    return true ;
  }



} // namespace mpm


