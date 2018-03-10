

#include "string.hh"


#define WHITE_SPACE " \t\r\v\n\f"

#include <algorithm>
#include <iostream>

using std::string ;
using std::vector ;


namespace mpm{
  string trim( string const& str ){
  	return trim( str, WHITE_SPACE );
  }

  string trim( string const& str, string const& chars ){
  	size_t first = str.find_first_not_of( chars ) ; 
  	size_t last = str.find_last_not_of( chars ) ; 

  	if ( first == string::npos || first > last ){ return string();}
  	return str.substr(first, 1 + last - first ) ; 
  }

  string to_lower( string const& str ){
  	string ans = str ; 
  	transform( str.begin(), str.end(), ans.begin(), ::tolower );
  	return ans; 
  }

  string to_upper( string const& str ){
  	string ans = str ; 
  	transform( str.begin(), str.end(), ans.begin(), ::toupper ) ; 
  	return ans; 
  }

  float to_float( string const& str ){
  	return to_num<float> ( str ) ; 
  }

  double to_double(string const& str ){
  	return to_num<double>( str ) ;
  }

  int64_t to_int( string const& str ){
  	return to_num<int>( str ) ; 
  }

  unsigned to_uint( string const& str ){
  	return to_num<unsigned>( str ) ; 
  }

  size_t to_size_t( string const& str ){
  	return to_num<size_t> ( str ) ; 
  }

  bool to_bool( string const& str ){
  	const string& s = to_lower( trim( str ) ) ; 
  	return !( s.empty() || s == "false" || s=="f" || s=="0" || s=="no" );
  }

  string canonicalize( string const& str ){
  	vector<string> ans ; 
  	split( str, ans ) ;
  	return to_lower( join(ans) ) ; 
  }

  void split( string const& str, vector<string>& ans ){
  	split( str, WHITE_SPACE, ans );
  }

  void split( string const& str, string const& separators, vector<string>& ans ){
  	size_t pos = 0 ; 
  	while( string::npos != ( pos = str.find_first_not_of(separators, pos ) ) ){
  		size_t const next_sep = str.find_first_of( separators, pos ) ; 
  		if ( string::npos == next_sep ){
  			ans.push_back( str.substr(pos) );
  			break;
  		}

  		ans.push_back( str.substr( pos, next_sep - pos ) ) ; 
  		pos = next_sep ; 
  	}
  }

  string join( vector<string> const& src ){
  	return join( src, " " );
  }

  string join( vector<string> const& src, string const& separator ){
  	if (src.empty()){ return "";}

  	std::stringstream ss(std::stringstream::out );

  	for (unsigned k = 0 ; ; ++k){
  		ss << src[k] ; 
  		if ( k+1 == src.size() ) {break;}
  		ss << separator ; 
  	}
  	return ss.str(); 
  }

  bool starts_with( string const& src, string const& start ){
  	if ( start.size() > src.size() ) { return false; }
  	return 0 == src.compare( 0, start.size(), start ) ; 
  }

  bool ends_with( string const& src, string const& end ){
  	if ( end.size() > src.size() ){ return false; }
  	return 0 == src.compare( src.size() - end.size(), string::npos, end ) ; 
  }

  

  bool split_on_next_marker ( const std::string &src, std::string &first_part, std::string &second_part )
  {
    size_t lm_start = 0, lm_end = 0, lm_idx = string::npos ;

    size_t pos = 0 ;
    while( string::npos != ( pos = src.find( '%', pos ) ) && ( pos+1 ) < src.size() )
    {
      if( src[pos+1] == '%' )
      {
        pos += 2 ;
        if( pos == src.size() ) break ;
        else                    continue ;
      }

      second_part = src.substr( pos+1, 64 ) ;
      std::istringstream is ( second_part ) ;
      size_t marker_idx ;
      is >> marker_idx ;

      if( marker_idx < lm_idx )
      {
        lm_start = pos ;
        lm_end = pos + 1 + second_part.size() - is.rdbuf()->in_avail() ;
        lm_idx = marker_idx ;
      }

      ++pos ;
    }

    if( lm_idx == string::npos )
    {
      return false ;
    }

    first_part = src.substr( 0, lm_start ) ;
    second_part = src.substr( lm_end ) ;

    return true ;
  }
  
}
