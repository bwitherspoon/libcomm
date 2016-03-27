# Copyright 2015 C. Brett Witherspoon

find_package(Boost 1.53 COMPONENTS program_options REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

if (CMAKE_USE_PTHREADS_INIT AND LIBUSB_FOUND)
    add_executable(usb_to_pipe usb_to_pipe.cpp)
    target_link_libraries(usb_to_pipe signum ${Boost_LIBRARIES})
    install(TARGETS usb_to_pipe DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

if (OPENCL_FOUND)
    add_executable(print_opencl_parameters print_opencl_parameters.cpp)
    target_link_libraries(print_opencl_parameters signum ${Boost_LIBRARIES})
    install(TARGETS print_opencl_parameters DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

