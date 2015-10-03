/*
 * Copyright 2015 C. Brett Witherspoon
 */

#define BOOST_TEST_MODULE comm_tests
#include <boost/test/unit_test.hpp>

#include "comm/message.hpp"

BOOST_AUTO_TEST_CASE(message_test)
{
  const bool b = true;
  const uint8_t ui8 = 55;
  const int8_t i8 = -9;
  const float f32 = 9.0;
  const int32_t i32 = -10000.7;

  bool b_r;
  uint8_t u8_r;
  int8_t ui8_r;
  float f32_r;
  int32_t i32_r;

  comm::message msg;

  msg.serialize();
  msg << b << ui8 << i8 << f32 << i32;
  msg.serialize();

  BOOST_REQUIRE_NO_THROW(msg.deserialize());

  msg >> b_r >> u8_r >> ui8_r >> f32_r >> i32_r;

  BOOST_CHECK_EQUAL(b_r, b);

  BOOST_CHECK_EQUAL(u8_r, ui8);

  BOOST_CHECK_EQUAL(ui8_r, i8);

  BOOST_CHECK_EQUAL(f32_r, f32);

  BOOST_CHECK_EQUAL(i32_r, i32_r);

  BOOST_REQUIRE_NO_THROW(msg.deserialize());
}
