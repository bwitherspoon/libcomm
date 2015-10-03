# Copyright 2015 C. Brett Witherspoon

find_package(Boost 1.53 COMPONENTS unit_test_framework REQUIRED)

add_definitions(-DBOOST_TEST_DYN_LINK)

include_directories(${Boost_INCLUDE_DIRS})

add_executable(message message.cpp)
target_link_libraries(message ${Boost_LIBRARIES})
add_test(message message)

add_executable(socket socket.cpp)
target_link_libraries(socket ${Boost_LIBRARIES} ${ZeroMQ_LIBRARIES})
target_include_directories(socket PRIVATE ${ZeroMQ_INCLUDE_DIRS})
add_test(socket socket)

add_executable(math math.cpp)
target_link_libraries(math ${Boost_LIBRARIES})
add_test(math math)

add_executable(buffer buffer.cpp)
target_link_libraries(buffer ${Boost_LIBRARIES})
add_test(buffer buffer)

add_executable(aligned_allocator aligned_allocator.cpp)
target_link_libraries(aligned_allocator ${Boost_LIBRARIES})
add_test(aligned_allocator aligned_allocator)

add_executable(oscillator oscillator.cpp)
target_link_libraries(oscillator ${Boost_LIBRARIES})
add_test(oscillator oscillator)

