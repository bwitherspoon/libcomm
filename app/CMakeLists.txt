# Copyright 2015 C. Brett Witherspoon

find_package(Boost 1.53 COMPONENTS program_options REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

if (CMAKE_USE_PTHREADS_INIT AND LIBUSB_FOUND)
    add_executable(usb_to_pipe usb_to_pipe.cpp)
    target_link_libraries(usb_to_pipe signum ${Boost_LIBRARIES})
    install(TARGETS usb_to_pipe DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

if (OPENCL_FOUND)
    add_executable(opencl_info opencl_info.cpp)
    target_link_libraries(opencl_info signum ${Boost_LIBRARIES})
    install(TARGETS opencl_info DESTINATION ${CMAKE_INSTALL_BINDIR})

    add_executable(opencl_fixed_to_float opencl_fixed_to_float.cpp)
    target_link_libraries(opencl_fixed_to_float signum ${Boost_LIBRARIES})
    install(TARGETS opencl_fixed_to_float DESTINATION ${CMAKE_INSTALL_BINDIR})
    install(FILES opencl_fixed_to_float.cl DESTINATION ${CMAKE_INSTALL_DATADIR})
endif()

