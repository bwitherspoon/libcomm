# Copyright 2015 C. Brett Witherspoon

find_package(Boost 1.53 COMPONENTS unit_test_framework REQUIRED)

add_definitions(-DBOOST_TEST_DYN_LINK)

include_directories(${Boost_INCLUDE_DIRS})

add_executable(message_test message_test.cpp)
target_link_libraries(message_test comm ${Boost_LIBRARIES})
add_test(message_test message_test)

add_executable(socket_test socket_test.cpp)
target_link_libraries(socket_test comm ${Boost_LIBRARIES})
target_include_directories(socket_test PRIVATE ${ZeroMQ_INCLUDE_DIRS})
add_test(socket_test socket_test)

add_executable(math_test math_test.cpp)
target_link_libraries(math_test ${Boost_LIBRARIES})
add_test(math_test math_test)

add_executable(fixed_test fixed_test.cpp)
target_link_libraries(fixed_test ${Boost_LIBRARIES})
add_test(fixed_test fixed_test)

add_executable(buffer_test buffer_test.cpp)
target_link_libraries(buffer_test comm ${Boost_LIBRARIES})
add_test(buffer_test buffer_test)

add_executable(aligned_allocator_test aligned_allocator_test.cpp)
target_link_libraries(aligned_allocator_test ${Boost_LIBRARIES})
add_test(aligned_allocator_test aligned_allocator_test)

add_executable(oscillator_test oscillator_test.cpp)
target_link_libraries(oscillator_test ${Boost_LIBRARIES})
add_test(oscillator_test oscillator_test)

add_executable(signal_test signal_test.cpp)
target_link_libraries(signal_test ${Boost_LIBRARIES})
add_test(signal_test signal_test)

