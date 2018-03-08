cmake_minimum_required(VERSION 2.8)

project(mpm_geo)
# set(CMAKE_BUILD_TYPE Release)

include_directories(../)
include_directories(../../third_party/eigen/)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_library(mpm_geo SHARED 
	./boundaryInfo.cpp
    ./meshLevelSet.cpp
    ./tensorField.cpp
    ./vectorField.cpp    	     
	./grid.cpp
    ./particles.cpp
    ./tet.cpp
    ./voxel.cpp    	             
	./levelSet.cpp
    ./scalarField.cpp
    ./tetGrid.cpp    	   
	./meshBase.cpp
    ./tensor.cpp
    ./triangularMesh.cpp    	        
	)