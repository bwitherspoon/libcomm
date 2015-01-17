/*
 * Copyright 2015 C. Brett Witherspoon
 */

#define BOOST_TEST_MODULE comm_tests
#include <boost/test/unit_test.hpp>

#include "comm/message.hpp"

BOOST_AUTO_TEST_CASE(message_test)
{
  bool b0 = true;
  bool b1 = false;
  uint8_t u8_0 = 55;

  comm::message msg;

  msg.pack().pack(b0).pack(u8_0);

  msg.unpack().unpack(b1);

  BOOST_CHECK_EQUAL(b0, b1);
}
