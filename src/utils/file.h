

#ifndef MPM_utils_file_h_
#define MPM_utils_file_h_

#include <string>
#include <vector>
#include <list>
#include <fstream>

#include "string.h"
#include "log.h"

#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>

using std::string ;

namespace mpm{

  class File: public std::fstream
  {
  public:
    explicit File( const std::string& name = "") ; 
    File( const std::string& name, const std::ios_base::openmode mode ) ; 

    virtual ~File(){}

    bool exists() const ; 

    bool open() { return open(_mode ); }
    bool open( std::string const& name ) { return open( name, _mode ); }
    bool open( std::ios_base::openmode mode ) ; 
    bool open( const std::string& name, const std::ios_base::openmode mode ) ;

    void setName( const std::string& name ){
      if ( !is_open() ){
        _name = name ; 
      }
    }

    const std::string& name() const{ return _name; }

    void setMode( std::ios_base::openmode mode ){
      if( !is_open() ){ _mode = mode ; }
    }

    static bool exists( std::string const& name ) ; 
    bool get_contents( std::string& contents );

  private:
    std::string _name ; 
    std::ios_base::openmode _mode ; 
  } ;


  class FileInfo
  {
  public:
    const static char* DIR_SEP ; 
    const static char* DIR_ROOT ; 
    const static char* DIR_CUR ; 

    explicit FileInfo( File& file );
    explicit FileInfo( const std::string& path = "");

    void setPath( const std::string& path ) ; 
    const std::string& path() const { return _path ; }
            
    bool exists() const; 
    bool isDir() const ; 

    void getAPath( std::vector<string>& path_elements ) const ;
    bool makePath() const;
    bool makeDir() const ; 

    FileInfo parentDirectory() const ; 
    std::string name() const ; 
    std::string absolutePath() const ; 

    std::string virtualAbsolutePath() const; 

    std::string filePath( const std::string& fileName )  const;

  private:
  	void print_error( const char* func, int errnum, bool win32_err = false) const;

  	std::string _path ; 
  	std::string _absolutePath ; 
  };

}



#endif