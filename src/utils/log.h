

#ifndef MPM_UTIL_LOG_H_
#define MPM_UTIL_LOG_H_

#include "logLevels.h"
#include "string.hh"
#include <mutex>
#include <iostream>
#include <sstream>

namespace mpm{
  namespace Log{

    struct Config{
      std::ostream* out  ;
      std::ostream* err ; 

      Level level ; 

      bool no_prefix ; 

      static Config& get() {
        static Config s_config ;
        return s_config ; 
      }
      
      void setLevel( std::string const& levelStr ){
        #define MPM_LOG_LEVEL( l ) \
          if ( levelStr == mpm_stringify( l )){ level = L_##l ; }
          MPM_LOG_LEVELS
        #undef MPM_LOG_LEVEL
      }

    private:
      // singleton
      Config()
      : out( nullptr ), err(nullptr), level(L_All), no_prefix(false)
      {}


    };// config


    namespace log_impl{

      template < Level L > struct Log ; 
      template < Level L > struct Stream{
        template<typename T>
        Log<L>& operator<<( const T& o ) const ; 


        Log<L>& operator<<( std::ostream& modifier(std::ostream& ) ) const ;

        static std::ostream& as_std_ostream() ;

        static std::ostream& unsafe() ;

        private:
          static Log<L>& impl() ; 
      };
    }


    // typedef for each log level
    #define MPM_LOG_LEVEL( level ) typedef log_impl::Stream< L_##level > level ;
      MPM_LOG_LEVELS
      typedef log_impl::Stream< L_Nothing > Nothing ;
    #undef MPM_LOG_LEVEL

    //! Full and abbreviated names
    template< Level L > struct  LevelNames {
      static const char* full() { return "Nothing" ; }
      static char abbrev() { return  'N' ; }
    } ;


     // Specializations for each Level
    #define MPM_LOG_LEVEL( level ) \
      template <> struct  LevelNames< L_##level > { \
        static const char* full() { return #level ; } \
        static char abbrev() { return  #level[0] ; } \
      } ;
      MPM_LOG_LEVELS
    #undef MPM_LOG_LEVEL



    namespace log_impl{

      struct LogBase{
        explicit LogBase( std::stringbuf *buf )
        :stream(buf)
        {
          Config &c = Config::get() ;
          if( !c.out ){
            c.out = &std::cout ;
          }
          if( !c.err ){
            c.err = &std::cerr ;
          }
        }

        template<Level l>
        static const char * prefix(){
          static const char s_prefix[5] = { '[', LevelNames< l >::abbrev(), ']', ' ', '\0' };
          return ( Config::get().no_prefix ) ? "" : s_prefix ;
        }
  
        static std::recursive_mutex& global_mutex(){
          static std::recursive_mutex s_global ;
          return s_global ;
        }
  
        std::ostream stream ;

      }; // LogBase


      template < Level L   >
      struct Buf : public std::stringbuf {
        std::ostream* &stream ;
        
        std::recursive_mutex mutex ;
        volatile bool locked ;

        Buf( std::ostream* &stream_ptr )
          : stream( stream_ptr )
        {}

        //! Called each time the stream is flushed ( e.g. for each std::endl )
        virtual int sync ( )
        {
          {
            //Output prefix + buffered string
            std::lock_guard<std::recursive_mutex> lock( LogBase::global_mutex() ) ;
           
            *stream << LogBase::prefix<L>( ) << str();
            stream->flush();
          }

          str("");

          // Release mutex
          if( locked ){
            locked = false ;
            mutex.unlock() ;
          }

          return 0;
        } // sync()

        virtual std::streamsize xsputn (const char* s, std::streamsize n)
        {
          acquire_mutex();
          return std::stringbuf::xsputn( s, n ) ;
        }

        void acquire_mutex()
        {
          mutex.lock() ;
          if( locked ) mutex.unlock() ; //This thread was already owning the recursive mutex, release it once

          locked = true ;
        }

      } ; // Buf



      template < Level L   >
      struct Log : public LogBase {
        Buf< L > buffer ;

        Log() : LogBase( &buffer ), buffer ( Config::get().out ) {}

        Log& thread_safe() { buffer.acquire_mutex() ; return *this ; }
          std::ostream& unsafe() { return  *buffer.stream << LogBase::prefix< L >() ; }
        } ;

        template < >
        struct Log< L_Error > : public LogBase {
          Buf< L_Error > buffer ;

          Log() : LogBase( &buffer ), buffer ( Config::get().err ) {}

          Log& thread_safe() { buffer.acquire_mutex() ; return *this ; }

          std::ostream& unsafe() { return *buffer.stream << LogBase::prefix< L_Error >() ; }
        } ;

        template < Level L, typename T >
          Log< L > & operator << ( Log< L > & s, const T& o )
        {
          if( L < Config::get().level ) return s ;
          s.thread_safe().stream << o ;
          return s ;
        }

        template < Level L >
        Log< L > & operator << ( Log< L > & s, std::ostream& modifier( std::ostream& )  )
        {
          if( L < Config::get().level ) return s ;
          s.thread_safe().stream << modifier ;
          return s ;
        }


        template < > struct Stream< L_Nothing >  {

        static std::ostream& as_std_ostream( ){
          return impl() ;
        }

        static std::ostream& unsafe( ){
          return as_std_ostream() ;
        }

      private:

        struct black_hole_buf : public std::streambuf {
          std::streamsize xsputn (const char *, std::streamsize n) override {
            return n;
          }
          int overflow (int) override {
            return 1;
          }
        };
        
        static std::ostream& impl() {
          static black_hole_buf s_buf ;
          static std::ostream s_stream(&s_buf) ;
          return s_stream ;
        }
        };


        //! Filtered, thread safe operator<<
        template< Level L >
        template< typename T >
        Log< L >& Stream< L >::operator<< (  const T& o ) const
        {
          return ( impl() << o ) ;
        }

        //! Filtered, thread safe operator<< for stream manipulators
        template< Level L >
        Log< L >& Stream< L >::operator<< ( std::ostream& modifier( std::ostream& )  ) const
        {
          return ( impl() << modifier ) ;
        }

        //! Returns an almost thread safe std::ostream
        /*! Actually, the rarely used sputc() method of the streambuf won't be thread safe */
        template< Level L >
        std::ostream& Stream< L >::as_std_ostream( )
        {
          if ( L < Config::get().level ){
            return Nothing::as_std_ostream() ;
          }
          return impl().thread_safe().stream ;
        }

        //! Thread-unsafe stream, with should not perform any blocking call to malloc
        template< Level L >
        std::ostream& Stream< L >::unsafe( )
        {
          if ( L < Config::get().level ){
            return Nothing::as_std_ostream() ;
          }

          return impl().unsafe() ;
        }

        template< Level L >
        Log< L >& Stream< L >::impl()
        {
          static Log< L > s_impl ;
          return s_impl ;
        }


    } // namespace log_impl


  } // namespace Log


} // namespace mpm


#endif