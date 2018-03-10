#include <gtest/gtest.h>

#include "geometry/triangularMesh.h"
#include "utils/file.h"

using namespace mpm ;

TEST( geo, mesh )
{
	const std::string &test_obj =
			FileInfo(__FILE__).parentDirectory().filePath("../scenes/bunny.obj") ;

	if( FileInfo(test_obj).exists() ) {

		TriangularMesh mesh ;
		ASSERT_TRUE( mesh.loadObj( test_obj.c_str() ) ) ;


		ASSERT_EQ( mesh.nFaces(), 5002u ) ;
		ASSERT_EQ( mesh.nVertices(), 2503u ) ;
		ASSERT_TRUE( mesh.hasVertexNormals() ) ;


		ASSERT_FALSE( mesh.hasVertexUVs() ) ;
		ASSERT_FALSE( mesh.hasFaceNormals() ) ;

		mesh.computeFaceNormals() ;
		ASSERT_TRUE( mesh.hasFaceNormals() ) ;

	}
}

