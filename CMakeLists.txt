# Copyright 2015 C. Brett Witherspoon

cmake_minimum_required(VERSION 2.8.11)
project(comm CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(ZeroMQ REQUIRED)

if (CMAKE_VERSION VERSION_LESS 2.8.12)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -std=gnu++11 -Werror -Wall -pipe")
else()
  add_compile_options(-march=native -std=gnu++11 -Werror -Wall -pipe)
endif()

message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

enable_testing()

include_directories(include)

add_subdirectory(test)
