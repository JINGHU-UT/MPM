

#ifndef MPM_BOUNDARY_INFO_H_
#define MPM_BOUNDARY_INFO_H_

#include "utils/alg.h"

#include <string>
#include <vector>

#include <unordered_map>


namespace mpm{
  struct BoundaryInfo
  {
    enum Bc {
      Interior,
      Stick,   // u = 0
      Slip,    // u.n = 0, d( (I - nn') u ).n = 0, (I - nn')sn = 0
      Normal,  // (I -nn') u = 0, d ( u.n ).n = 0, (nn')sn = 0
      Free,    // d( u ).n = 0, sn = 0
      Corner
    };

    Bc bc ;
    Vec3 normal ;

    BoundaryInfo() :
      bc( Interior )
    {}

    BoundaryInfo( const Bc bc_, const Vec3 n )
      : bc(bc_), normal(n)
    {}

    void set( const Bc bc_, const Vec3 n )
    {
      bc = bc_ ;
      normal = n ;
    }

    void combine( const Bc bc_, const Vec3 n ) ;
    static BoundaryInfo combine(const BoundaryInfo &b1, const BoundaryInfo &b2 ) ;

    void    velProj( Mat3 &proj ) const ;
    void   spinProj( Mat3 &proj ) const ;
    void stressProj( Mat6 &proj ) const ;
  };






  typedef std::vector< BoundaryInfo > BoundaryConditions ;

  struct BoundaryMapper {
    virtual BoundaryInfo::Bc operator() ( const std::string & /*domain*/ ) const
    { return BoundaryInfo::Stick ; }
  };



  class StrBoundaryMapper : public BoundaryMapper
  {
  public:

    explicit StrBoundaryMapper( const std::string & str ) ;

    virtual BoundaryInfo::Bc operator() ( const std::string &domain ) const ;

  private:

    BoundaryInfo::Bc from_string( const std::string &bc ) ;

    typedef std::unordered_map< std::string, BoundaryInfo::Bc > Map ;
    
    Map m_bc ;

  };


} // ns mpm



#endif