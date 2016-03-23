/*
 * Copyright 2015 C. Brett Witherspoon
 */

#include <zmq.h>

#include "signum/socket.hpp"

namespace signum {

context::context()
{
  m_context = zmq_ctx_new();
  if (m_context == nullptr)
    throw socket::socket_error(__func__);
}

context::~context()
{
  zmq_ctx_destroy(m_context);
}

std::unique_ptr<socket> context::make_socket(socket::types type)
{
  return std::unique_ptr<socket>(new socket(m_context, type));
}

} /* namespace signum */
