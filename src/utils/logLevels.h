

#ifndef MPM_UTILS_LOGLEVELS_H_
#define MPM_UTILS_LOGLEVELS_H_


#define MPM_LOG_LEVELS \
  MPM_LOG_LEVEL( All ) \
  MPM_LOG_LEVEL( Debug ) \
  MPM_LOG_LEVEL( Verbose ) \
  MPM_LOG_LEVEL( Info ) \
  MPM_LOG_LEVEL( Warning ) \
  MPM_LOG_LEVEL( Error )

  namespace mpm{
    namespace Log {

      enum Level{
        #define MPM_LOG_LEVEL( level ) L_##level, 
          MPM_LOG_LEVELS
        #undef MPM_LOG_LEVEL

        L_Nothing,
      };

      

    } // end namespace log
  } // end namespace mpm



#endif



  // refer: https://stackoverflow.com/questions/6503586/what-does-in-a-define-mean
  // The MACRO is equivalent to 
  //    L_All
  //    L_Debug
  //    L_Verbose
  //    L_Info
  //    L_Warning
  //    L_Error