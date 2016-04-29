# Copyright 2015, 2016 C. Brett Witherspoon

cmake_minimum_required(VERSION 2.8.12)

if (DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING
        "Choose the type of build: Debug Release ReleaseWithDebInfo MinSizeRel")
else()
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
        "Choose the type of build: Debug Release ReleaseWithDebInfo MinSizeRel")
endif()

project(signum C CXX)
enable_testing()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

include(GNUInstallDirs)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})

add_compile_options(-march=native -std=gnu++14 -Werror -Wall -pipe)

set(THREADS_PREFER_PTHREAD_FLAG)
find_package(Threads REQUIRED)
find_package(Doxygen)
find_package(ZeroMQ)
find_package(libusb-1.0)
find_package(OpenCL)

include_directories(include)

set(HEADERS
    include/signum/aligned_allocator.hpp
    include/signum/buffer.hpp
    include/signum/fixed.hpp
    include/signum/math.hpp
    include/signum/message.hpp
    include/signum/oscillator.hpp
    include/signum/rational_resampler.hpp
    include/signum/signal.hpp
    include/signum/utility/endian.hpp
    include/signum/pipe.hpp)

set(SOURCES
    src/utility/endian.cpp
    src/message.cpp
    src/buffer.cpp)

if (ZEROMQ_FOUND)
    list(APPEND SOURCES src/zeromq/context.cpp src/zeromq/socket.cpp)
    list(APPEND HEADERS include/signum/zeromq/socket.cpp)
    include_directories(${ZeroMQ_INCLUDE_DIRS})
else()
    message(WARNING "Building without ZeroMQ support")
endif()

if (LIBUSB_FOUND)
    list(APPEND HEADERS
         include/signum/usb/context.hpp
         include/signum/usb/device.hpp
         include/signum/usb/session.hpp
         include/signum/usb/transfer.hpp)
    include_directories(${LIBUSB_INCLUDE_DIRS})
else()
    message(WARNING "Building without USB support")
endif()

if (CMAKE_USE_PTHREADS_INIT)
    list(APPEND SOURCES src/utility/priority.cpp)
    list(APPEND HEADERS include/signum/utility/priority.cpp)
else()
    message(WARNING "Building without POSIX threads support")
endif()

if (OPENCL_FOUND)
    list(APPEND SOURCES src/opencl/fft.cpp)
    list(APPEND HEADERS include/signum/opencl/fft.hpp)
    include_directories(${OpenCL_INCLUDE_DIRS} /usr/include/compute /usr/local/include/compute)
else()
    message(WARNING "Building without OpenCL support")
endif()

if (DOXYGEN_FOUND)
    add_subdirectory(doc)
else()
    message(WARNING "Building without Doxygen support")
endif()

add_subdirectory(examples)
add_subdirectory(test)

add_library(signum SHARED ${SOURCES})

target_link_libraries(signum ${CMAKE_THREAD_LIBS_INIT})
if (ZEROMQ_FOUND)
    target_link_libraries(signum ${ZeroMQ_LIBRARIES})
endif()
if (LIBUSB_FOUND)
    target_link_libraries(signum ${LIBUSB_LIBRARIES})
endif()
if (OPENCL_FOUND)
    target_link_libraries(signum ${OpenCL_LIBRARIES})
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/signum.pc.in
               ${CMAKE_CURRENT_BINARY_DIR}/signum.pc
               @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/signum.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
install(TARGETS signum DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

