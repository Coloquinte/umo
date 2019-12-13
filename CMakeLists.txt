
cmake_minimum_required(VERSION 2.8)
PROJECT(UMO)

SET(CMAKE_CXX_FLAGS "-g -Wall -Werror -std=c++14")

INCLUDE_DIRECTORIES(
    ${UMO_SOURCE_DIR}/include
)

SET(SOURCES
  src/api/umo_api.cpp
  src/model/model.cpp
)

add_library(libumo ${SOURCES})

