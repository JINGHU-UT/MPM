
#include "levelSet.impl.h"

#include "triangularMesh.h"

#include <iostream>

namespace mpm {

MeshLevelSet::MeshLevelSet( const char* objFile )
  : m_objFile( objFile ),
    m_radius( 1.e-1 ),
    m_grid( Vec3::Ones(), Vec3i::Constant( 50 ) ),
    m_values( m_grid )
{}

Scalar MeshLevelSet::eval_local(const Vec3 &x) const {
  Vec3 x_loc = x - m_offset ;
  if(! x_loc.isApprox( m_grid.clamp_point( x_loc ) ) ) {
    return - m_emptyVal ;
  }
  return - m_values( x_loc ) ;
}

Vec3 MeshLevelSet::grad_local(const Vec3 &x) const {
  Vec3 x_loc = x - m_offset ;
  if(! x_loc.isApprox( m_grid.clamp_point( x_loc ) ) ) {
    return Vec3::Zero() ;
  }
  return - m_values.grad_at( x_loc );
}

static Vec3   closestPointOnTriangle( const Vec3& p, const Mat3& T ) ;
static Scalar signedDistPointTriangle(const Vec3& P, const Mat3& T, const TriangularMesh &mesh, const ID face) ;
static Vec3   barycentricCoordinates(const Vec3& p, const Mat3& T ) ;

bool MeshLevelSet::compute()
{
  TriangularMesh triMesh ;

  if (! triMesh.loadObj( m_objFile.c_str() ) )
    return false ;

  triMesh.computeFaceNormals() ;

  // AABB
  Arr3 bbMax = triMesh.vertices().rowwise().maxCoeff() ;
  Arr3 bbMin = triMesh.vertices().rowwise().minCoeff() ;

  const Scalar absRad = (bbMax - bbMin).matrix().norm() * m_radius ;
  bbMax += absRad ;
  bbMin -= absRad ;

  m_grid.set_box( bbMax - bbMin ) ;
  m_offset = bbMin ;

  const Scalar emptyVal = absRad / m_radius ;
  m_values.set_constant( emptyVal ) ;

  // Splat Triangles on grid
  unsigned nF = triMesh.nFaces() ;
  for( unsigned i = 0 ; i < nF ; ++i )
  {
    Mat3 vertices ;
    vertices.col(0) = triMesh.vertex( i, 0 ) - m_offset ;
    vertices.col(1) = triMesh.vertex( i, 1 ) - m_offset ;
    vertices.col(2) = triMesh.vertex( i, 2 ) - m_offset ;

    Arr3 bbMax = vertices.rowwise().maxCoeff().array() + absRad ;
    Arr3 bbMin = vertices.rowwise().minCoeff().array() - absRad ;

    Grid::Location maxCell, minCell ;
    m_grid.locate( bbMax.matrix(), maxCell ) ;
    m_grid.locate( bbMin.matrix(), minCell ) ;

    for( ID j = minCell.cell[0] ; j <= maxCell.cell[0]+1 ; ++j ) {
      for( ID k = minCell.cell[1] ; k <= maxCell.cell[1]+1 ; ++k ) {
        for( ID l = minCell.cell[2] ; l <= maxCell.cell[2]+1 ; ++l ) {
          Grid::Vertex node( j, k, l ) ;
          const Scalar d = signedDistPointTriangle(
            m_grid.nodePosition( node ), vertices, triMesh, i ) ;

          Scalar &val = m_values[ m_grid.nodeID(node) ] ;
          if( std::fabs(d) < std::fabs(val) )
            val = d ;
        }
      }
    }

  }

  // Correct signs of far-away cells

#pragma omp parallel for
  for( ID j = 0 ; j < m_grid.dim()[0]+1 ; ++ j ) {
    for( ID k = 0 ; k < m_grid.dim()[1]+1 ; ++ k ) {

      int sign = 1 ; //Default to outside

      for( ID l = 0 ; l < m_grid.dim()[2]+1 ; ++ l ) {
        Grid::Vertex node( j, k, l ) ;
        Scalar &val = m_values[ m_grid.nodeID(node) ] ;
        if( val != emptyVal ) sign = val>0 ? 1 : -1 ;
        val = std::min( std::fabs( val ), absRad ) * sign ;
      }
    }
  }

  m_emptyVal = absRad ;

  return true ;
}

static Scalar signedDistPointTriangle(const Vec3& P, const Mat3& T, const TriangularMesh &mesh, const ID face )
{
  const Vec3 proj = closestPointOnTriangle( P, T ) ;
  const Scalar dist = (P - proj).norm() ;
  const Vec3 bc = barycentricCoordinates( proj, T ) ;
  const Vec3 N  = mesh.interpolatedNormal( face, bc ) ;
  return (P - proj).dot( N ) > 0 ? dist : -dist ;
}

// From Christer Ericson -- Real-Time Collision Detection
static Vec3 closestPointOnTriangle(const Vec3& p, const Mat3& T )
{
  const Mat3::ConstColXpr a = T.col(0) ;
  const Mat3::ConstColXpr b = T.col(1) ;
  const Mat3::ConstColXpr c = T.col(2) ;

   // Check if P in vertex region outside A
   Vec3 ap = p - a;
   Vec3 ab = b - a;
   Vec3 ac = c - a;

   Scalar d1 = ab.dot(ap);
   Scalar d2 = ac.dot(ap);
   if (d1 <= 0. && d2 <= 0.) return a; // barycentric coordinates (1,0,0)
   // Check if P in vertex region outside B
   Vec3 bp = p - b;
   Scalar d3 = ab.dot(bp);
   Scalar d4 = ac.dot(bp);
   if (d3 >= 0. && d4 <= d3) return b; // barycentric coordinates (0,1,0)
   // Check if P in edge region of AB, if so return projection of P onto AB
   Scalar vc = d1*d4 - d3*d2;
   if (vc <= 0. && d1 >= 0. && d3 <= 0.) {
      Scalar v = d1 / (d1 - d3);
      return a + v * ab; // barycentric coordinates (1-v,v,0)
   }
   // Check if P in vertex region outside C
   Vec3 cp = p - c;
   Scalar d5 = ab.dot(cp);
   Scalar d6 = ac.dot(cp);
   if (d6 >= 0. && d5 <= d6) return c; // barycentric coordinates (0,0,1)
   // Check if P in edge region of AC, if so return projection of P onto AC
   Scalar vb = d5*d2 - d1*d6;
   if (vb <= 0. && d2 >= 0. && d6 <= 0.) {
      Scalar w = d2 / (d2 - d6);
      return a + w * ac; // barycentric coordinates (1-w,0,w)
   }
   // Check if P in edge region of BC, if so return projection of P onto BC
   Scalar va = d3*d6 - d5*d4;
   if (va <= 0. && (d4 - d3) >= 0. && (d5 - d6) >= 0.) {
      Scalar w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
      return b + w * (c - b); // barycentric coordinates (0,1-w,w)
   }
   // P inside face region. Compute Q through its barycentric coordinates (u,v,w)
   Scalar denom = 1. / (va + vb + vc);
   Scalar v = vb * denom;
   Scalar w = vc * denom;
   return a + ab * v + ac * w; // = u*a + v*b + w*c, u = va * denom = 1.0d - v - w
}

// Compute barycentric coordinates (u, v, w) for
// point p with respect to triangle (a, b, c)
static Vec3 barycentricCoordinates(const Vec3& p, const Mat3& T )
{
  const Mat3::ConstColXpr a = T.col(0) ;
  const Mat3::ConstColXpr b = T.col(1) ;
  const Mat3::ConstColXpr c = T.col(2) ;

  Vec3 v0 = b - a, v1 = c - a, v2 = p - a;
  const Scalar d00 = v0.dot(v0);
  const Scalar d01 = v0.dot(v1);
  const Scalar d11 = v1.dot(v1);
  const Scalar d20 = v2.dot(v0);
  const Scalar d21 = v2.dot(v1);
  const Scalar denom = d00 * d11 - d01 * d01;

  Vec3 coords ;
  coords[1] = (d11 * d20 - d01 * d21) / denom;
  coords[2] = (d00 * d21 - d01 * d20) / denom;
  coords[0] = 1.0f - coords[1] - coords[2];

  return coords ;
}

} //mpm

