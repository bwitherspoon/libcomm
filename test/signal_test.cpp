/*
 * Copyright 2015 C. Brett Witherspoon
 */

#define BOOST_TEST_MODULE signum_test
#include <boost/test/unit_test.hpp>

#include <numeric>
#include <vector>

#include "signum/signal.hpp"

BOOST_AUTO_TEST_CASE(signal_empty_test)
{
  signum::signal<float> sig;

  BOOST_REQUIRE(sig.empty());

  BOOST_CHECK_EQUAL(sig.size(), 0);
}

BOOST_AUTO_TEST_CASE(signal_vector_test)
{
  std::vector<int> vec(10);

  signum::signal<int> sig(vec);

  std::iota(vec.begin(), vec.end(), -1);

  BOOST_CHECK_EQUAL(vec.size(), sig.size());

  BOOST_CHECK_EQUAL_COLLECTIONS(vec.begin(), vec.end(), sig.begin(), sig.end());
}
