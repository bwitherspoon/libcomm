/*
 * Copyright 2015 C. Brett Witherspoon
 */

#define BOOST_TEST_MODULE signum_tests
#include <boost/test/unit_test.hpp>

#include "signum/message.hpp"

BOOST_AUTO_TEST_CASE(message_test)
{
  const bool b = true;
  const uint8_t u8 = 55;
  const int8_t s8 = -9;
  const float f32 = 9.0;
  const int32_t s32 = -10000.7;

  bool b_r;
  uint8_t u8_r;
  int8_t s8_r;
  float f32_r;
  int32_t s32_r;

  signum::message msg;

  BOOST_REQUIRE_NO_THROW(msg.serialize());
  BOOST_REQUIRE_NO_THROW(msg << b);
  BOOST_REQUIRE_NO_THROW(msg << u8);
  BOOST_REQUIRE_NO_THROW(msg << s8);
  BOOST_REQUIRE_NO_THROW(msg << f32);
  BOOST_REQUIRE_NO_THROW(msg << s32);
  BOOST_REQUIRE_NO_THROW(msg.serialize());

  BOOST_REQUIRE_NO_THROW(msg.deserialize());
  BOOST_REQUIRE_NO_THROW(msg >> b_r);
  BOOST_REQUIRE_NO_THROW(msg >> u8_r);
  BOOST_REQUIRE_NO_THROW(msg >> s8_r);
  BOOST_REQUIRE_NO_THROW(msg >> f32_r);
  BOOST_REQUIRE_NO_THROW(msg >> s32_r);
  BOOST_REQUIRE_NO_THROW(msg.deserialize());

  BOOST_CHECK_EQUAL(b_r, b);

  BOOST_CHECK_EQUAL(u8_r, u8);

  BOOST_CHECK_EQUAL(s8_r, s8);

  BOOST_CHECK_EQUAL(f32_r, f32);

  BOOST_CHECK_EQUAL(s32_r, s32);
}
