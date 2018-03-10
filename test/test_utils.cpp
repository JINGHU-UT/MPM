
#include "utils/alg.h"

#include "utils/string.hh"
#include "utils/file.h"
#include "utils/config.h"

#include <gtest/gtest.h>

using namespace mpm ;

TEST( utils, alg )
{
	constexpr ID D = 6 ;
	const ID m = 5 ;

	DynVec s(m) ;
	for( int i = 0 ; i < m ; ++i ) s[i] = i+1 ;

	DynVec t ;
	set_compwise<D>( t, s ) ;

	for( int i = 0 ; i < m ; ++i ) {
		EXPECT_TRUE( Segmenter<D>::segment(t,i).isApprox( Vec6::Constant(s[i]) ) ) ;
	}

	component<D>(t, 0) *= 2 ;

	for( int i = 0 ; i < m ; ++i ) {
		EXPECT_DOUBLE_EQ( 2.*s[i], Segmenter<D>::segment(t,i)[0] ) ;
	}

	s = component<D>((const DynVec&) t, 0 ) ;
	div_compwise<D>( t, s ) ;

	for( int i = 0 ; i < m ; ++i ) {
		EXPECT_DOUBLE_EQ( 1., Segmenter<D>::segment(t,i)[0] ) ;
		EXPECT_DOUBLE_EQ( (D+1.)/2, Segmenter<D>::segment(t,i).sum() ) ;
	}

	mul_compwise<D>( t, s ) ;
	for( int i = 0 ; i < m ; ++i ) {
		EXPECT_DOUBLE_EQ( s[i], Segmenter<D>::segment(t,i)[0] ) ;
	}
}

TEST( utils, config )
{
	const std::string &test_scene =
			FileInfo(__FILE__).parentDirectory().filePath("../scenes/test.conf") ;

	Config c ;
	ASSERT_TRUE( c.from_file( test_scene ) ) ;

	ASSERT_EQ( c.res, Vec3i(1, 5, 10) ) ;
	ASSERT_TRUE( c.gravity.isApprox( Vec3(40,0,0) ) ) ;
	ASSERT_TRUE( c.box.isApprox( Vec3(10,20,30) ) ) ;
	ASSERT_DOUBLE_EQ( c.fps * c.substeps, 1.e2 ) ;

	const Scalar Re = ( c.box.minCoeff() * std::sqrt( c.box.minCoeff() * c.gravity.norm()) * c.volMass ) / c.viscosity ;
	ASSERT_DOUBLE_EQ( 2.e8, Re ) ;

	c.internalize();

	ASSERT_TRUE( c.box.isApprox( Vec3(10,20,30) ) ) ;
	ASSERT_TRUE( c.gravity.isApprox( Vec3(1,0,0) ) ) ;
//	ASSERT_DOUBLE_EQ( 20. , c.units().U ) ;
//	ASSERT_DOUBLE_EQ( .5  , c.units().T ) ;
//	ASSERT_DOUBLE_EQ( 4.e5, c.units().P ) ;
//	ASSERT_DOUBLE_EQ( 2.e5, c.units().M ) ;



	ASSERT_DOUBLE_EQ( c.units().M, c.units().R * c.units().U * c.units().L ) ;

	ASSERT_DOUBLE_EQ( c.viscosity, c.box.minCoeff() * std::sqrt( c.box.minCoeff() ) * 1./Re ) ;

}
