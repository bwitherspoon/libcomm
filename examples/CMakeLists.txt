# Copyright 2015 C. Brett Witherspoon

find_package(Boost 1.53 COMPONENTS program_options REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

if (CMAKE_USE_PTHREADS_INIT AND LIBUSB_FOUND)
    add_executable(usb_to_udp usb_to_udp.cpp)
    target_link_libraries(usb_to_udp signum ${Boost_LIBRARIES})
    install(TARGETS usb_to_udp DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

if (OPENCL_FOUND)
    add_executable(fft_benchmark fft_benchmark.cpp)
    target_link_libraries(fft_benchmark signum ${OpenCL_LIBRARIES} ${Boost_LIBRARIES})
    install(TARGETS fft_benchmark DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
