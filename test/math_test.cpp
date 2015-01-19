/*
 * Copyright 2015 C. Brett Witherspoon
 */

#define BOOST_TEST_MODULE comm_tests
#include <boost/test/unit_test.hpp>

#include "comm/math.hpp"

BOOST_AUTO_TEST_CASE(abs_test)
{
  using comm::math::abs;

  BOOST_CHECK_EQUAL(abs(-5), 5);

  BOOST_CHECK_EQUAL(abs(12), 12);
}

BOOST_AUTO_TEST_CASE(gcd_test)
{
  using comm::math::gcd;

  BOOST_CHECK_EQUAL(gcd(6, 15), 3);
}

BOOST_AUTO_TEST_CASE(lcm_test)
{
  using comm::math::lcm;

  BOOST_CHECK_EQUAL(lcm(4, 10), 20);
}
