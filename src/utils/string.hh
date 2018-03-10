
#ifndef mpm_utils_STRING_H_
#define mpm_utils_STRING_H_


#include <string>
#include <vector>
#include <sstream>


#include "alg.h"

namespace mpm{

  // remove white spaces
  std::string trim( std::string const& str ); 

  // Main the middle, remove chars at both ends
  std::string trim( std::string const& str, std::string const& chars );

  std::string to_lower( std::string const& );
  std::string to_upper( std::string const& );

  float to_float( std::string const& );
  double to_double( std::string const& );
  int64_t to_int( std::string const& );
  unsigned to_uint( std::string const& );
  std::size_t to_size_t( std::string const& );
  bool to_bool( std::string const& );

  std::string canonicalize( std::string const& ) ; 
  
  void split( std::string const& str, std::string const& separators, 
  	std::vector<std::string>& ans);
  void split( std::string const& str, std::vector<std::string>& ans); 

  std::string join( std::vector<std::string> const& src, std::string const& separators );
  std::string join( std::vector<std::string> const& src ) ; 

  // whether src starts with "start"
  bool starts_with( std::string const& src, std::string const& start);
  bool ends_with( std::string const& src, std::string const& end);


  template< typename T >
  bool cast ( std::istringstream& stream, T& res ){
  	return static_cast<bool>( stream >> res ) ;
  }

  template< >
  inline bool cast ( std::istringstream& stream, std::string& res ){
  	std::string remaining ;
  	if( ! std::getline( stream, remaining ) )
  		return false ;
  	res = trim( remaining );
  	return true ;
  }

  template< typename Scalar, int Rows, int Cols >
  bool cast ( std::istringstream& stream, Eigen::Matrix<Scalar, Rows, Cols>& res )
  {
    for( int k = 0 ; stream && ( k < res.size() ) ; ++ k) {
      stream >> res.data()[k] ;
    }
    return static_cast<bool>( stream ) ;
  }

  template< typename T >
  bool cast ( std::string const& str, T & ans ){
  	std::istringstream stream(str);
  	return cast(stream, ans) ; 
  }

  template< typename T >
  std::ostream& dump ( std::ostream& stream, const T& val ){
  	return ( stream << val ) ;
  }

  template< typename Scalar, int Rows, int Cols >
  std::ostream& dump ( std::ostream& stream, const Eigen::Matrix<Scalar, Rows, Cols>& val )
  {
    for( int k = 0 ; stream && ( k < val.size() ) ; ++ k) {
      stream << val.data()[k] << "\t" ;
    }
    return stream ;
  }

  template< typename T_num >
  T_num to_num ( const std::string& str ){
  	T_num n ;
  	return cast( str, n ) ? n : 0 ;
  }




  bool split_on_next_marker ( const std::string &src, std::string &first_part, std::string &second_part ) ;
  template< typename T >
  std::ostream& farg( std::ostream &os, const std::string &src, const T &replacement )
  {
    std::string p1, p2 ;
    if( split_on_next_marker( src, p1, p2 ) )
    {
      os << p1 << replacement << p2 ;
    }
    return os ;
  }

  template< typename T >
  std::stringstream& farg( std::stringstream &os, const T &replacement )
  {
    std::string src = os.str() ;
    os.str("") ;
    farg< T >( os, src, replacement ) ;
    return os ;
  }

  template< typename T >
  std::string arg( const std::string &src, const T &replacement )
  {
    std::stringstream os( std::stringstream::out ) ;
    os.str( src );
    return farg<T>( os, replacement).str() ;
  }
  template< typename T1, typename T2 >
  std::string arg( const std::string &src, const T1 &r1, const T2& r2 )
  {
    std::stringstream os( std::stringstream::out ) ;
    os.str( src );
    return farg< T2 >( farg< T1 >( os, r1 ), r2 ).str() ;
  }

  template< typename T1, typename T2, typename T3 >
  std::string arg3( const std::string &src, const T1 &r1, const T2& r2, const T3& r3 )
  {
    std::stringstream os( std::stringstream::out ) ;
    os.str( src );
    return farg< T3 >( farg< T2 >( farg< T1 >( os, r1 ), r2 ), r3 ).str() ;
  }

} // namespace mpm


#define mpm_stringify(s) mpm_preproc_str(s)
#define mpm_preproc_str(s) #s



#endif


