/*
 * Copyright 2015 C. Brett Witherspoon
 */

#ifndef SIGNUM_ZEROMQ_SOCKET_HPP_
#define SIGNUM_ZEROMQ_SOCKET_HPP_

#include <memory>
#include <stdexcept>
#include <string>

namespace signum { class message; }

namespace signum
{
namespace zeromq
{

//! A messaging socket
class socket
{
public:
  friend class context;

  enum class types
  {
    request = 3,
    reply   = 4
  };

  //! An exception thrown on message socket errors
  struct socket_error : public std::runtime_error
  {
    explicit socket_error(const std::string& where, const std::string& what);
    explicit socket_error(const std::string& where);
  };

  //! A socket can not be copy constructed
  socket(const socket&) = delete;

  //! A socket can not be copy assigned
  socket& operator=(const socket&) = delete;

  //! A socket can not be move constructed
  socket(socket&&) = delete;

  //! A socket can not be move assigned
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
  socket& send(const signum::message& msg);

  /*! \brief Receives a message on the socket
   *  \param msg a message
   *  \sa send()
   */
  socket& recv(signum::message& msg);

private:
  /*!
   * \brief Constructs a message socket
   * \param context a context
   * \param type a socket type
   */
  socket(void* context, types type);

  void* m_socket; //!< A ZeroMQ socket
};

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
   *  \param type a message socket type
   *  \returns A new message socket
   */
  std::unique_ptr<socket> make_socket(socket::types type);

private:
  void* m_context;
};

} /* namespace zeromq */
} /* namespace signum */

#endif /* SIGNUM_ZEROMQ_SOCKET_HPP_ */
