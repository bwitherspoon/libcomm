# Copyright 2015 C. Brett Witherspoon

cmake_minimum_required(VERSION 2.8.12)

if (DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING
        "Choose the type of build: Debug Release ReleaseWithDebInfo MinSizeRel")
else()
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
        "Choose the type of build: Debug Release ReleaseWithDebInfo MinSizeRel")
endif()

project(comm CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

include(GNUInstallDirs)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})

add_compile_options(-march=native -std=gnu++14 -Werror -Wall -pipe)

include_directories(include ${LIBUSB_INCLUDE_DIRS} ${ZeroMQ_INCLUDE_DIRS})

find_package(Doxygen)
find_package(Threads REQUIRED)
find_package(ZeroMQ REQUIRED)
find_package(libusb-1.0 REQUIRED)

add_library(
    comm
    SHARED
    src/context.cpp
    src/endian.cpp
    src/message.cpp
    src/socket.cpp
    src/buffer.cpp
    src/usb.cpp)
target_link_libraries(
    comm
    ${ZeroMQ_LIBRARIES}
    ${LIBUSB_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)

install(TARGETS comm DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

if(DOXYGEN_FOUND)
    add_subdirectory(doc)
endif()

add_subdirectory(app)

enable_testing()
add_subdirectory(test)

message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

