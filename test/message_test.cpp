/*
 * Copyright 2015 C. Brett Witherspoon
 */

#define BOOST_TEST_MODULE comm_tests
#include <boost/test/unit_test.hpp>

#include "comm/message.hpp"

BOOST_AUTO_TEST_CASE(message_test)
{
  const bool b = true;
  const uint8_t u8 = 55;
  const uint64_t u64 = 678;

  comm::message msg;

  msg.pack(b).pack().pack(u8).pack().pack(u64).pack();

  BOOST_REQUIRE_EQUAL(msg.unpack<bool>(), b);
  BOOST_REQUIRE_NO_THROW(msg.unpack());
  BOOST_REQUIRE_EQUAL(msg.unpack<uint64_t>(), u8);
  BOOST_REQUIRE_NO_THROW(msg.unpack());
  BOOST_REQUIRE_EQUAL(msg.unpack<uint64_t>(), u64);
  BOOST_REQUIRE_NO_THROW(msg.unpack());
}
