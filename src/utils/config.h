

#ifndef MPM__UTIL_CONFIG_H_
#define MPM__UTIL_CONFIG_H_

#include "utils/units.h"
#include "utils/alg.h"

#include <string>
#include <cmath>
#include <fstream>
#include <regex>

namespace mpm {
  
  #define EXPAND_CONFIG \
    CONFIG_FIELD( fps            , Scalar      , Units::Frequency    ) \
    CONFIG_FIELD( substeps       , unsigned    , Units::None         ) \
    CONFIG_FIELD( nFrames        , unsigned    , Units::None         ) \
    \
    CONFIG_FIELD( box            , Vec3        , Units::Length       ) \
    CONFIG_FIELD( res            , Vec3i       , Units::None         ) \
    CONFIG_FIELD( nSamples       , unsigned    , Units::None         ) \
    CONFIG_FIELD( randomize      , Scalar      , Units::None         ) \
    \
    CONFIG_FIELD( volMass        , Scalar      , Units::VolumicMass  ) \
    CONFIG_FIELD( viscosity      , Scalar      , Units::Viscosity    ) \
    CONFIG_FIELD( gravity        , Vec3        , Units::Acceleration ) \
    CONFIG_FIELD( phiMax         , Scalar      , Units::None         ) \
    \
    CONFIG_FIELD( mu             , Scalar      , Units::None         ) \
    \
    CONFIG_FIELD( delta_mu       , Scalar      , Units::None         ) \
    CONFIG_FIELD( I0             , Scalar      , Units::None         ) \
    CONFIG_FIELD( grainDiameter  , Scalar      , Units::Length       ) \
    CONFIG_FIELD( muRigid        , Scalar      , Units::None         ) \
    \
    CONFIG_FIELD( cohesion       , Scalar      , Units::Stress       ) \
    CONFIG_FIELD( cohesion_decay , Scalar      , Units::None         ) \
    \
    CONFIG_FIELD( anisotropy     , Scalar      , Units::None         ) \
    CONFIG_FIELD( elongation     , Scalar      , Units::None         ) \
    CONFIG_FIELD( brownian       , Scalar      , Units::None         ) \
    CONFIG_FIELD( initialOri     , Vec3        , Units::None         ) \
    \
    CONFIG_FIELD( enforceMaxFrac , bool        , Units::None         ) \
    CONFIG_FIELD( weakStressBC   , bool        , Units::None         ) \
    \
    CONFIG_FIELD( scenario       , std::string , Units::None         ) \
    CONFIG_FIELD( boundary       , std::string , Units::None         ) \
    CONFIG_FIELD( output         , bool        , Units::None         ) \
    CONFIG_FIELD( dumpPrimalData , unsigned    , Units::None         ) \
  
  
  struct Config
  {
    Config( ) ;
  
    bool from_string( const std::string &key, const std::string &value ) ;
    bool from_file(const std::string& file_name) ;
  
    bool dump( const std::string& file_name, const char* comment = nullptr ) const ;
  
    // Transform parameters from SI to internal units
    void internalize() ;
  
    const Units& units() const {
      return m_units ;
    }
  
    Scalar time( unsigned frame_nb ) const {
      return (frame_nb / fps ) ;
    }
  
  #define CONFIG_FIELD( name, type, u ) \
    type name ;
    EXPAND_CONFIG
  #undef CONFIG_FIELD
  
  private:
    bool from_string( const std::string &key, std::istringstream &value ) ;
  
    Units m_units ;
  
  };

} // mpm




#endif