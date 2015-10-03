/*
 * Copyright 2015 C. Brett Witherspoon
 */

#define BOOST_TEST_MODULE comm_tests
#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <functional>
#include <random>
#include <vector>

#include "comm/aligned_allocator.hpp"

BOOST_AUTO_TEST_CASE(aligned_allocator_test)
{
  const std::size_t size = 2048;

  std::vector<float> ref(size);
  std::vector<float, comm::aligned_allocator<float>> out(size);

  std::random_device dev;
  std::default_random_engine eng(dev());
  std::uniform_real_distribution<> dist(0.0, 1.0);
  auto rand = std::bind(dist, eng);

  std::generate(ref.begin(), ref.end(), rand);
  std::copy(ref.begin(), ref.end(), out.begin());

  BOOST_CHECK_EQUAL_COLLECTIONS(ref.begin(), ref.end(), out.begin(), out.end());
}
