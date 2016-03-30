/*
 * Copyright 2015 C. Brett Witherspoon
 */

#ifndef SIGNUM_MESSAGE_HPP_
#define SIGNUM_MESSAGE_HPP_

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace signum
{

namespace zeromq { class socket; }

/**
 * \brief A class for serializing and deserializing messages
 */
class message
{
public:
  friend class zeromq::socket;

  //! The binary message formats
  enum class formats : uint8_t
  {
    posfixint = 0x00,
    fixmap    = 0x80,
    fixarray  = 0x90,
    fixstr    = 0xa0,
    nil       = 0xc0,
    invalid   = 0xc1,
    boolfalse = 0xc2,
    booltrue  = 0xc3,
    bin8      = 0xc4,
    bin16     = 0xc5,
    bin32     = 0xc6,
    ext8      = 0xc7,
    ext16     = 0xc8,
    ext32     = 0xc9,
    float32   = 0xca,
    float64   = 0xcb,
    uint8     = 0xcc,
    uint16    = 0xcd,
    uint32    = 0xce,
    uint64    = 0xcf,
    int8      = 0xd0,
    int16     = 0xd1,
    int32     = 0xd2,
    int64     = 0xd3,
    fixext1   = 0xd4,
    fixext2   = 0xd5,
    fixext4   = 0xd6,
    fixext8   = 0xd7,
    fixext16  = 0xd8,
    str8      = 0xd9,
    str16     = 0xda,
    str32     = 0xdb,
    array16   = 0xdc,
    array32   = 0xdd,
    map16     = 0xde,
    map32     = 0xdf,
    negfixint = 0xe0
  };

  //! An exception thrown on deserialization errors
  struct deserialize_error : public std::runtime_error
  {
    deserialize_error(const std::string& loc, const std::string& msg);
    deserialize_error(const char* loc, const char* msg);
  };

  //! Messages cannot be copy constructed
  message(const message&) = delete;

  //! Messages cannot be copy assigned
  message& operator=(const message&) = delete;

  //! Construct an empty message
  message();

  //! Move construct a message
  message(message&& other);

  //! Move assign a message
  message& operator=(message&& other);

  //! Deconstruct a message
  ~message() = default;

  //! Set the message to an empty state
  void clear();

  //! Reset the message so it can be serialized again
  void reset() { m_pos = 0; }

  //! Serialize a nil type
  message& serialize();

  //! Serialize a boolean type
  message& serialize(bool value);

  //! Serialize a 64 bit unsigned integer type
  message& serialize(uint64_t value);

  //! Serialize a 32 bit unsigned integer type
  message& serialize(uint32_t value);

  //! Serialize a 16 bit unsigned integer type
  message& serialize(uint16_t value);

  //! Serialize a 8 bit unsigned integer type
  message& serialize(uint8_t value);

  //! Serialize a 64 bit signed integer type
  message& serialize(int64_t value);

  //! Serialize a 32 bit signed integer type
  message& serialize(int32_t value);

  //! Serialize a 16 bit signed integer type
  message& serialize(int16_t value);

  //! Serialize a 8 bit signed integer type
  message& serialize(int8_t value);

  //! Serialize a single precision floating point type
  message& serialize(float value);

  //! Serialize a double precision floating point type
  message& serialize(double value);

  //! Serialize any supported type
  template<typename T>
  message& operator<<(T value) { return serialize(value); }

  //! Deserialize a nil type
  message& deserialize();

  //! Deserialize any supported type
  template<typename T> message& deserialize(T& value);

  //! Deserialize any supported type
  template<typename T>
  message& operator>>(T& value) { return deserialize(value); }

private:
  uint64_t deserialize_format(uint64_t value, uint8_t format);

  uint32_t deserialize_format(uint32_t value, uint8_t format);

  uint16_t deserialize_format(uint16_t value, uint8_t format);

  uint8_t deserialize_format(uint8_t value, uint8_t format);

  int64_t deserialize_format(int64_t value, uint8_t format);

  int32_t deserialize_format(int32_t value, uint8_t format);

  int16_t deserialize_format(int16_t value, uint8_t format);

  int8_t deserialize_format(int8_t value, uint8_t format);

  double deserialize_format(double value, uint8_t format);

  float deserialize_format(float value, uint8_t format);

  uint8_t* data() { return m_data.data(); }

  const uint8_t* data() const { return m_data.data(); }

  std::size_t size() const { return m_data.size(); }

  void resize(std::size_t count) { m_data.resize(count); }

  //! Insert a byte into the message buffer
  void insert(uint8_t data);

  //! Insert a format byte into the message buffer
  void insert(formats data);

  //! Insert data into the message buffer
  template<typename T> void insert(const T& data);

  //! Extract a single byte from the message buffer without advancing position
  uint8_t extract();

  //! Extract data from the message buffer without advancing position
  template<typename T> T& extract(T&);

  std::vector<uint8_t> m_data;           //!< message buffer
  std::vector<uint8_t>::size_type m_pos; //!< extract position
};

////////////////////////////////////////////////////////////////////////////////

template<typename T>
message& message::deserialize(T& value)
{
  auto format = extract();

  m_pos++;

  try
  {
    value = deserialize_format(value, format);
  }
  catch (const deserialize_error&)
  {
    m_pos--;
    throw;
  }

  return *this;
}

template<>
message& message::deserialize<bool>(bool& value);

template<typename T>
void message::insert(const T& data)
{
  const uint8_t* first = reinterpret_cast<const uint8_t*>(&data);

  m_data.insert(std::end(m_data), first, first + sizeof(data));
}

template<typename T>
T& message::extract(T& data)
{
  if (m_data.size() - m_pos < sizeof(data))
    throw deserialize_error(__func__, "message size insufficient");

  uint8_t* result = reinterpret_cast<uint8_t*>(&data);

  std::copy_n(&m_data[m_pos], sizeof(data), result);

  return data;
}

////////////////////////////////////////////////////////////////////////////////

constexpr uint8_t operator|(message::formats lhs, uint8_t rhs)
{
  return static_cast<uint8_t>(lhs) | rhs;
}

constexpr uint8_t operator|(uint8_t lhs, message::formats rhs)
{
  return rhs | lhs;
}

constexpr uint8_t operator&(message::formats lhs, uint8_t rhs)
{
  return static_cast<uint8_t>(lhs) & rhs;
}

constexpr uint8_t operator&(uint8_t lhs, message::formats rhs)
{
  return rhs & lhs;
}

constexpr bool operator==(message::formats lhs, uint8_t rhs)
{
  return static_cast<uint8_t>(lhs) == rhs;
}

constexpr bool operator==(uint8_t lhs, message::formats rhs)
{
  return rhs == lhs;
}

constexpr bool operator!=(message::formats lhs, uint8_t rhs)
{
  return !(lhs == rhs);
}

constexpr bool operator!=(uint8_t lhs, message::formats rhs)
{
  return rhs != lhs;
}

} /* namespace signum */

#endif /* SIGNUM_MESSAGE_HPP_ */
