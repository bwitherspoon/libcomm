/*
 * Copyright 2015 C. Brett Witherspoon
 */

#ifndef INCLUDE_MESSAGE_HPP_
#define INCLUDE_MESSAGE_HPP_

#include <endian.h>
#include <algorithm>
#include <cstdint>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace comm
{

//! An exception thrown on deserialization errors
struct unpack_error : public std::runtime_error
{
  explicit unpack_error(const std::string& what_arg) :
      std::runtime_error(what_arg)
  {
  }
  explicit unpack_error(const char* what_arg) :
      std::runtime_error(what_arg)
  {
  }
};

/**
 * \brief A class for serializing and deserializing messages
 *
 *  See https://github.com/msgpack/msgpack/blob/master/spec.md
 */
class message
{
public:
  enum class format : uint8_t
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

  message(const message&) = delete;

  ~message() = default;

  message& operator=(const message&) = delete;

  //! Construct an empty message
  message();

  //! Move constructs a message
  message(message&& other);

  //! Move assigns a message
  message& operator=(message&& other);

  //! Sets the message to an empty state
  void clear();

  //! Serialize a nil type
  message& pack();

  //! Serialize a boolean type
  message& pack(bool value);

  //! Serialize a 64 bit unsigned integer type
  message& pack(uint64_t value);

  //! Serialize a 32 bit unsigned integer type
  message& pack(uint32_t value);

  //! Serialize a 16 bit unsigned integer type
  message& pack(uint16_t value);

  //! Serialize a 8 bit unsigned integer type
  message& pack(uint8_t value);

  //! Serialize a signed integer type
  message& pack(int64_t value);

  //! Serialize a signed integer type
  message& pack(int32_t value);

  //! Serialize a signed integer type
  message& pack(int16_t value);

  //! Serialize a signed integer type
  message& pack(int8_t value);

  //! Serialize a single-precision floating-point type
  message& pack(float value);

  //! Serialize a double-precision floating-point type
  message& pack(double value);

  //! Deserialize a nil type
  message& unpack();

  //! Deserialize a boolean type
  message& unpack(bool& value);

private:
  //! A type trait helper
  template<typename T>
  struct is_data : std::integral_constant<bool,
      std::numeric_limits<T>::is_integer
          and not std::numeric_limits<T>::is_signed>
  {
  };

  //! Insert data into the message
  template<typename T>
  message& insert(T data);

  //! Extract data from the message
  template<typename T>
  T extract();

  std::vector<uint8_t> m_data;           //!< message data
  std::vector<uint8_t>::size_type m_pos; //!< extract position
};

////////////////////////////////////////////////////////////////////////////////

constexpr uint8_t operator|(message::format lhs, uint8_t rhs)
{
  return static_cast<uint8_t>(lhs) | rhs;
}

constexpr uint8_t operator|(uint8_t lhs, message::format rhs)
{
  return lhs | rhs;
}

constexpr bool operator==(message::format lhs, uint8_t rhs)
{
  return static_cast<uint8_t>(lhs) == rhs;
}

constexpr bool operator==(uint8_t lhs, message::format rhs)
{
  return rhs == lhs;
}

constexpr bool operator!=(message::format lhs, uint8_t rhs)
{
  return !(lhs == rhs);
}

constexpr bool operator!=(uint8_t lhs, message::format rhs)
{
  return rhs != lhs;
}

////////////////////////////////////////////////////////////////////////////////

template<typename T>
message& message::insert(T data)
{
  static_assert(
      is_data<T>::value,
      "The data type being inserted must be an unsigned integer type");
  const uint8_t* first = reinterpret_cast<const uint8_t*>(&data);
  const uint8_t* last = first + sizeof(T);
  m_data.insert(std::end(m_data), first, last);
  return *this;
}

template<>
message& message::insert<uint8_t>(uint8_t data)
{
  m_data.push_back(data);
  return *this;
}

template<>
message& message::insert<message::format>(format data)
{
  static_assert(
      std::is_same<uint8_t, std::underlying_type<format>::type>::value,
      "The underlying type of the format enum class is not uint8_t");
  insert(static_cast<uint8_t>(data));
  return *this;
}

template<typename T>
T message::extract()
{
}

template<>
uint8_t message::extract()
{
  uint8_t data;
  try
  {
    data = m_data.at(m_pos);
  }
  catch (std::out_of_range&)
  {
    throw unpack_error(__func__ + std::string("message size insufficient"));
  }
  return data;
}

////////////////////////////////////////////////////////////////////////////////

message::message() :
    m_data(), m_pos(0)
{
}

message::message(message&& other) :
    m_data(std::move(other.m_data)),
    m_pos(other.m_pos)
{
}

message& message::operator=(message&& other)
{
  m_data = std::move(other.m_data);
  m_pos = other.m_pos;
  return *this;
}

void message::clear()
{
  m_data.clear();
  m_pos = 0;
}

message& message::pack()
{
  insert(format::nil);
  return *this;
}

message& message::pack(bool value)
{
  insert((value) ? format::booltrue : format::boolfalse);
  return *this;
}

message& message::pack(uint64_t value)
{
  if (value > std::numeric_limits<uint32_t>::max())
  {
    insert(format::uint64);
    insert(htobe64(value));
  }
  else
  {
    pack(static_cast<uint32_t>(value));
  }
  return *this;
}

message& message::pack(uint32_t value)
{
  if (value > std::numeric_limits<uint16_t>::max())
  {
    insert(format::uint32);
    insert(htobe32(value));
  }
  else
  {
    pack(static_cast<uint16_t>(value));
  }
  return *this;
}

message& message::pack(uint16_t value)
{
  if (value > std::numeric_limits<uint8_t>::max())
  {
    insert(format::uint16);
    insert(htobe16(value));
  }
  else
  {
    pack(static_cast<uint8_t>(value));
  }
  return *this;
}

message& message::pack(uint8_t value)
{
  if (value < (1 << 7))
  {
    insert(format::posfixint | value);
  }
  else
  {
    insert(format::uint8);
    insert(value);
  }
  return *this;
}

message& message::pack(int64_t value)
{
  constexpr int32_t min = std::numeric_limits<int32_t>::min();
  constexpr int32_t max = std::numeric_limits<int32_t>::max();
  if (value < min or value > max)
  {
    insert(format::int64);
    insert(htobe64((uint64_t)value));
  }
  else
  {
    pack(static_cast<int32_t>(value));
  }
  return *this;
}

message& message::pack(int32_t value)
{
  constexpr int16_t min = std::numeric_limits<int16_t>::min();
  constexpr int16_t max = std::numeric_limits<int16_t>::max();
  if (value < min or value > max)
  {
    insert(format::int32);
    insert(htobe32((uint32_t)value));
  }
  else
  {
    pack(static_cast<int16_t>(value));
  }
  return *this;
}

message& message::pack(int16_t value)
{
  constexpr int8_t min = std::numeric_limits<int8_t>::min();
  constexpr int8_t max = std::numeric_limits<int8_t>::max();
  if (value < min or value > max)
  {
    insert(format::int16);
    insert(htobe16((uint16_t)value));
  }
  else
  {
    pack(static_cast<int8_t>(value));
  }
  return *this;
}

message& message::pack(int8_t value)
{
  if (value < -(1 << 5))
  {
    insert(format::int8);
    insert((uint8_t)value);
  }
  else
  {
    insert((uint8_t)value);
  }
  return *this;
}

message& message::pack(float value)
{
  static_assert(
      std::numeric_limits<float>::is_iec559,
      "The single precision floating point type is not IEEE 754");
  insert(format::float32);
  insert(htobe32((uint32_t)value));
  return *this;
}

message& message::pack(double value)
{
  static_assert(
      std::numeric_limits<double>::is_iec559,
      "The double precision floating point type is not IEEE 754");
  insert(format::float64);
  insert(htobe64((uint64_t)value));
  return *this;
}

message& message::unpack()
{
  uint8_t fmt = extract<uint8_t>();
  if (fmt != format::nil)
    throw unpack_error(__func__ + std::string(": format is not nil type"));
  m_pos += 1;
  return *this;
}

message& message::unpack(bool& value)
{
  uint8_t fmt = extract<uint8_t>();
  if (fmt == format::boolfalse)
    value = false;
  else if (fmt == format::booltrue)
    value = true;
  else
    throw unpack_error(__func__ + std::string(": format is not boolean type"));
  m_pos += 1;
  return *this;
}

} /* namespace comm */

#endif /* INCLUDE_MESSAGE_HPP_ */
