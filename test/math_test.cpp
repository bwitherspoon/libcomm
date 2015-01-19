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

  BOOST_CHECK_EQUAL(abs(-1.3), 1.3);

  BOOST_CHECK_EQUAL(abs(5.4), 5.4);
}

BOOST_AUTO_TEST_CASE(max_test)
{
  using comm::math::max;

  BOOST_CHECK_EQUAL(max(-7, 5), 5);

  BOOST_CHECK_EQUAL(max(7, 67), 67);

  BOOST_CHECK_EQUAL(max(-9.0, 5.7), 5.7);
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
