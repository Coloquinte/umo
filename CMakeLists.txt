
cmake_minimum_required(VERSION 2.8)
PROJECT(UMO)

SET(CMAKE_CXX_FLAGS "-g -Wall -std=c++14")

INCLUDE_DIRECTORIES(
    ${UMO_SOURCE_DIR}/include
)

SET(SOURCES
  src/api/c_api.cpp
  src/api/cpp_api.cpp
  src/model/model.cpp
  src/model/operator.cpp
  src/model/presolved_model.cpp
  src/presolve/presolve.cpp
  src/presolve/cleanup.cpp
  src/presolve/flatten.cpp
  src/presolve/propagate_constants.cpp
  src/presolve/to_linear.cpp
  src/presolve/to_sat.cpp
)

add_library(umo SHARED ${SOURCES})

enable_testing()
add_subdirectory(test)

