project(mpm)
cmake_minimum_required(VERSION 2.8)

OPTION( TESTS "Build tests" ON )
OPTION( APPS "Build apps" ON )
OPTION( LIB "Build lib" ON )

OPTION( OPENMP  "Use OpenMP" ON )
OPTION( TETGRID "Use regulard grid of tetrehedra" OFF )

if (TETGRID)
	add_definitions( -DMPM_MESH_IMPL=1 )
endif()

SET( MPM_ROOT ${CMAKE_CURRENT_SOURCE_DIR} )
SET( CMAKE_MODULE_PATH ${MPM_ROOT}/cmake )

# Eigen
FIND_PACKAGE(Eigen3 3.1.0 REQUIRED)
SET( EXTERNAL_INCLUDES ${EIGEN3_INCLUDE_DIR} )
SET( EXTERNAL_LIBRARIES "" )

# OpenMP
if( OPENMP )
	FIND_PACKAGE(OpenMP)
	if(OPENMP_FOUND)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
		add_definitions(-DEIGEN_DONT_PARALLELIZE)
	else()
		set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas " )
	endif()
endif()

#Bogus
find_package( SoBogus REQUIRED )
SET( EXTERNAL_INCLUDES ${EXTERNAL_INCLUDES} ${SoBogus_INCLUDE_DIR} )
add_definitions( -DBOGUS_WITH_BOOST_SERIALIZATION )

# boost
find_package(Boost 1.36.0 COMPONENTS serialization REQUIRED )
SET( EXTERNAL_INCLUDES ${EXTERNAL_INCLUDES} ${Boost_INCLUDE_DIR} )
SET( EXTERNAL_LIBRARIES ${EXTERNAL_LIBRARIES} ${Boost_LIBRARIES} )

#OpenGL
find_package(OpenGL)

# COmpiler flags
if (NOT CMAKE_BUILD_TYPE)
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE "Release")
endif()

if ( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" )
  set (CLANG TRUE)
  include_directories( /usr/lib/clang/3.7/include )
  set( CMAKE_EXPORT_COMPILE_COMMANDS 1 )
elseif( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" )
  set (GCC TRUE)
endif()

if ( GCC OR CLANG )
				SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra " )
				SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-deprecated-declarations " )
				SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Werror " )

				if( GCC AND APPLE )
						  SET( EXTERNAL_LIBRARIES ${EXTERNAL_LIBRARIES} stdc++ )
				endif()

				SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}  -march=native -DEIGEN_NO_DEBUG ")
endif()

#

include_directories( ${EXTERNAL_INCLUDES} )

if( LIB )
	add_subdirectory( src )
endif()

if( TESTS )
				enable_testing()
				add_subdirectory( test )
endif()

if( APPS )
				add_subdirectory( demo )
endif()
