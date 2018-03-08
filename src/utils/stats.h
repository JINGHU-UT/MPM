#ifndef mpm_stats_h_
#define mpm_stats_h_

#include "utils/file.h"
#include "utils/alg.h"

namespace mpm{

// A good example about how to use MACRO.
// Learn a lot. from Yuan

  #define EXPAND_STAT \
    STAT_FIELD( stepId             , unsigned , "step"    ) \
    STAT_FIELD( frameId            , unsigned , "frame"   ) \
    STAT_FIELD( delta_t            , Scalar   , "dt"      ) \
    STAT_FIELD( nParticles         , unsigned , "nPart"   ) \
    STAT_FIELD( nNodes             , unsigned , "totNds"  ) \
    STAT_FIELD( nActiveNodes       , unsigned , "actNds"  ) \
    STAT_FIELD( nCouplingNodes     , unsigned , "cplNds"  ) \
    STAT_FIELD( maxVelocity        , Scalar   , "maxVel"  ) \
    STAT_FIELD( frictionError      , Scalar   , "slvErr"  ) \
    STAT_FIELD( frictionIterations , unsigned , "slvIter" ) \
    STAT_FIELD( assemblyTime       , Scalar   , "asmTime" ) \
    STAT_FIELD( linSolveTime       , Scalar   , "linTime" ) \
    STAT_FIELD( lcpSolveTime       , Scalar   , "lcpTime" ) \
    STAT_FIELD( frictionTime       , Scalar   , "slvTime" ) \
    STAT_FIELD( advectionTime      , Scalar   , "advTime" ) \
    STAT_FIELD( totalTime          , Scalar   , "totTime" ) \

  
  class Stats
  {
  public:
  	Stats( const char* base_dir ) ; 

  	void dump() ;
  private:
  	File m_out ; 

  public:
  	#define STAT_FIELD( name, type, abv ) \
      type name ; 
      EXPAND_STAT
    #undef STAT_FIELD
  	
  }; // end class Stats

} // end namespace mpm



#endif
