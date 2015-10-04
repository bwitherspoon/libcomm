/*
 * Copyright 2015 C. Brett Witherspoon
 */

#include <zmq.h>
#include "comm/socket.hpp"
#include "comm/message.hpp"

namespace comm
{
socket::socket(void* context, socket::types type)
{
  m_socket = zmq_socket(context, static_cast<int>(type));
  if (m_socket == nullptr)
    throw socket_error(__func__);
}

socket::~socket()
{
  const int ms = 500;
  zmq_setsockopt(m_socket, ZMQ_LINGER, &ms, sizeof(int));
  zmq_close(m_socket);
}

socket& socket::bind(const std::string& addr)
{
  if (zmq_bind(m_socket, addr.c_str()) != 0)
    throw socket_error(__func__);

  return *this;
}

socket& socket::connect(const std::string& addr)
{
  if (zmq_connect(m_socket, addr.c_str()) != 0 )
    throw socket_error(__func__);

  return *this;
}

socket& socket::send(const message& msg)
{
  auto data = msg.data();
  auto size = msg.size();

  if (zmq_send(m_socket, data, size, 0) == -1)
    throw socket_error(__func__);

  return *this;
}

socket& socket::recv(message& msg)
{
  const std::size_t size = 1024;
  msg.resize(size);

  auto data = msg.data();

  int count = zmq_recv(m_socket, data, size, 0);

  if (count < 0)
    throw socket_error(__func__);

  if (static_cast<std::size_t>(count) > size)
    throw socket_error(__func__, "message truncated");

  msg.resize(static_cast<std::size_t>(count));
  msg.reset();

  return *this;
}

socket::socket_error::socket_error(
    const std::string& where, const std::string& what)
        : std::runtime_error(where + ": " + what)
{ }
socket::socket_error::socket_error(const std::string& where)
        : socket_error(where, zmq_strerror(zmq_errno()))
{ }
} /* namespace comm */

