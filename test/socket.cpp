/*
 * Copyright 2015 C. Brett Witherspoon
 */

#define BOOST_TEST_MODULE comm_test
#include <boost/test/unit_test.hpp>

#include "comm/socket.hpp"

BOOST_AUTO_TEST_CASE(socket_test)
{
  using comm::context;
  using comm::socket;
  using comm::message;

  context ctx;
  message msg;
  auto req = ctx.make_socket(socket::types::request);
  auto rep = ctx.make_socket(socket::types::reply);

  rep->bind("tcp://127.0.0.1:5555");
  req->connect("tcp://127.0.0.1:5555");

  int obj = -1;

  msg << obj;
  req->send(msg);

  rep->recv(msg);
  msg >> obj;

  BOOST_CHECK_EQUAL(obj, -1);

  msg.clear();
  msg << 0;
  rep->send(msg);
}
