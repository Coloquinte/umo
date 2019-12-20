
cmake_minimum_required(VERSION 2.8)
PROJECT(UMO)

SET(CMAKE_CXX_FLAGS "-g -Wall -std=c++14")

INCLUDE_DIRECTORIES(
    ${UMO_SOURCE_DIR}/include
)

SET(SOURCES
  src/api/c_api.cpp
  src/model/model.cpp
)

add_library(libumo ${SOURCES})

enable_testing()
add_subdirectory(test)

