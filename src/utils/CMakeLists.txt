cmake_minimum_required(VERSION 2.8)

project(mpm_utils)
# set(CMAKE_BUILD_TYPE Release)

include_directories(../)
include_directories(../../third_party/eigen/)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_library(mpm_utils SHARED 
	./config.cpp
    ./file.cpp
    ./stats.cpp
    ./string.cpp
    ./units.cpp
	)