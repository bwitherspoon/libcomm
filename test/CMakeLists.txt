# Copyright 2015 C. Brett Witherspoon

find_package(Boost 1.53 COMPONENTS unit_test_framework REQUIRED)

add_definitions(-DBOOST_TEST_DYN_LINK)

include_directories(${Boost_INCLUDE_DIRS})

add_executable(message_test message_test.cpp)
target_link_libraries(message_test ${Boost_LIBRARIES})
add_test(message_test message_test)

