
cmake_minimum_required(VERSION 2.8)
PROJECT(UMO)

macro(use_cxx11)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    endif ()
  else ()
    set (CMAKE_CXX_STANDARD 11)
  endif ()
endmacro(use_cxx11)

use_cxx11()

INCLUDE_DIRECTORIES(
    ${UMO_SOURCE_DIR}/include
)

SET(SOURCES
  src/api/c_api.cpp
  src/api/cpp_api.cpp
  src/model/model.cpp
  src/model/writer_umo.cpp
  src/model/writer_lp.cpp
  src/model/writer_cnf.cpp
  src/model/operator.cpp
  src/model/presolved_model.cpp
  src/presolve/presolve.cpp
  src/presolve/cleanup.cpp
  src/presolve/flatten.cpp
  src/presolve/propagate_constants.cpp
  src/presolve/to_linear.cpp
  src/presolve/to_sat.cpp
  src/solver/solver.cpp
  src/solver/external_solvers.cpp
)

add_library(umo STATIC ${SOURCES})
set_property(TARGET umo PROPERTY POSITION_INDEPENDENT_CODE ON)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    enable_testing()
    add_subdirectory(test)
endif()

