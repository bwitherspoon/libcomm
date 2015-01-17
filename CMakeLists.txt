# Copyright 2015 C. Brett Witherspoon

cmake_minimum_required(VERSION 2.8.11)
project(comm CXX)

enable_testing()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set(COMM_CXX_FLAGS "-march=native -std=gnu++11 -Werror -Wall -pipe")

if (CMAKE_VERSION VERSION_LESS 2.8.12)
  set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${COMM_CXX_FLAGS})
else()
  add_compile_options(${COMM_CXX_FLAGS})
endif()

message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

include_directories(include)

add_subdirectory(test)
