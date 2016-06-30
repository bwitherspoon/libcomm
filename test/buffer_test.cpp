/*
 * Copyright 2015. 2016 C. Brett Witherspoon
 */

#define BOOST_TEST_MODULE buffer_test
#include <boost/test/unit_test.hpp>

#include <array>
#include <numeric>
#include <vector>

#include "signum/buffer.hpp"

BOOST_AUTO_TEST_CASE(buffer_empty_test)
{
  signum::buffer<float> buf;

  BOOST_REQUIRE(buf.empty());

  BOOST_CHECK_EQUAL(buf.size(), 0);
}

BOOST_AUTO_TEST_CASE(buffer_vector_test)
{
  std::vector<int> vec(10);

  signum::buffer<int> buf(vec);

  std::iota(vec.begin(), vec.end(), -1);

  BOOST_CHECK_EQUAL(vec.size(), buf.size());

  BOOST_CHECK_EQUAL_COLLECTIONS(vec.begin(), vec.end(), buf.begin(), buf.end());
}

BOOST_AUTO_TEST_CASE(buffer_array_test)
{
  std::array<int, 10> arr;

  signum::buffer<int> buf(arr);

  std::iota(arr.begin(), arr.end(), -1);

  BOOST_CHECK_EQUAL(arr.size(), buf.size());

  BOOST_CHECK_EQUAL_COLLECTIONS(arr.begin(), arr.end(), buf.begin(), buf.end());
}

