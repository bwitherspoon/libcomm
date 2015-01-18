/*
 * Copyright 2015 C. Brett Witherspoon
 */

#ifndef INCLUDE_COMM_SOCKET_HPP_
#define INCLUDE_COMM_SOCKET_HPP_

#include <zmq.h>
#include <memory>
#include <stdexcept>
#include <string>

#include "comm/message.hpp"

namespace comm
{
//! An exception thrown on message socket errors
struct socket_error : public std::runtime_error
{
  explicit socket_error(const std::string& where, const std::string& what) :
      std::runtime_error(where + ": " + what)
  {
  }
  explicit socket_error(const std::string& where) :
      socket_error(where, zmq_strerror(zmq_errno()))
  {
  }
};

////////////////////////////////////////////////////////////////////////////////

//! A messaging socket
class socket
{
  friend class context;
public:
  enum class types
  {
    request = ZMQ_REQ,
    reply   = ZMQ_REP
  };

  socket(const socket&) = delete;

  socket& operator=(const socket&) = delete;

  socket(socket&&)  = delete;

  socket& operator=(socket&& other) = delete;

  //! Deconstructs a message socket
  ~socket();

  /*!
   * \brief Assigns an address to the message socket
   * \param addr an address
   * \sa connect()
   */
  socket& bind(const std::string& addr);

  /*!
   * \brief Connects the message socket to an address
   * \param addr an address
   * \sa bind()
   */
  socket& connect(const std::string& addr);

  /*! \brief Sends a message on the socket
   *  \param msg a message
   *  \sa recv()
   */
  socket& send(const message& msg);

  /*! \brief Receives a message on the socket
   *  \param msg a message
   *  \sa send()
   */
  socket& recv(message& msg);

private:
  /*!
   * \brief Constructs a message socket
   * \param context a context
   * \param type a socket type
   */
  socket(void* context, types type);

  void* m_socket; //!< A ZeroMQ socket
};

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

////////////////////////////////////////////////////////////////////////////////

/*! \brief A messaging context
 *
 *  A factory for constructing message sockets which all share the same context.
 *  A messaging context is thread safe, but the sockets it creates are not. All
 *  sockets created by context must be destroyed before the deconstructor of the
 *  context is called or it will block.
 */
class context
{
public:
  //! Constructs a new messaging context
  context();

  /*!
   * \brief Deconstructs the messaging context
   *
   * After the context is destroyed any sockets it constructed will throw an
   * exception if blocked or used.
   */
  ~context();

  /*! \brief Constructs a new message socket
   *  \param type A message socket type
   *  \returns A new message socket
   */
  std::unique_ptr<socket> make_socket(socket::types type);

private:
  void* m_context;
};

context::context()
{
  m_context = zmq_ctx_new();
  if (m_context == nullptr)
    throw socket_error(__func__);
}

context::~context()
{
  zmq_ctx_destroy(m_context);
}

std::unique_ptr<socket> context::make_socket(socket::types type)
{
  return std::unique_ptr<socket>(new socket(m_context, type));
}

} /* namespace comm */

#endif /* INCLUDE_COMM_SOCKET_HPP_ */
