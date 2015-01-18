# Copyright 2015 C. Brett Witherspoon

find_package(Boost 1.53 COMPONENTS unit_test_framework REQUIRED)

add_definitions(-DBOOST_TEST_DYN_LINK)

include_directories(${Boost_INCLUDE_DIRS})

add_executable(message_test message_test.cpp)
target_link_libraries(message_test ${Boost_LIBRARIES})
add_test(message_test message_test)

add_executable(socket_test socket_test.cpp)
target_link_libraries(socket_test ${Boost_LIBRARIES} ${ZeroMQ_LIBRARIES})
target_include_directories(socket_test PRIVATE ${ZeroMQ_INCLUDE_DIRS})
add_test(socket_test socket_test)

