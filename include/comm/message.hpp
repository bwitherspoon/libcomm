/*
 * Copyright 2015 C. Brett Witherspoon
 */

#ifndef INCLUDE_MESSAGE_HPP_
#define INCLUDE_MESSAGE_HPP_

#include <endian.h>
#include <algorithm>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace comm
{

/**
 * \brief A class for serializing and deserializing messages
 *
 *  See https://github.com/msgpack/msgpack/blob/master/spec.md for the
 *  serialization format.
 */
class message
{
public:
  //! A message binary format
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

  //! An exception thrown on deserialization errors
  struct unpack_error : public std::runtime_error
  {
    explicit unpack_error(const std::string& loc, const std::string& msg) :
        std::runtime_error(loc + ": " + msg)
    {
    }
    explicit unpack_error(const char* loc, const char* msg) :
        unpack_error(std::string(loc), std::string(msg))
    {
    }
  };

  message(const message&) = delete;

  ~message() = default;

  message& operator=(const message&) = delete;

  //! Construct an empty message
  message();

  //! Move construct a message
  message(message&& other);

  //! Move assign a message
  message& operator=(message&& other);

  //! Set the message to an empty state
  void clear();

  //! Reset the message so it can be unpacked again
  void reset() { m_pos = 0; }

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

  //! Serialize a 64 bit signed integer type
  message& pack(int64_t value);

  //! Serialize a 32 bit signed integer type
  message& pack(int32_t value);

  //! Serialize a 16 bit signed integer type
  message& pack(int16_t value);

  //! Serialize a 8 bit signed integer type
  message& pack(int8_t value);

  //! Serialize a single precision floating point type
  message& pack(float value);

  //! Serialize a double precision floating point type
  message& pack(double value);

  //! Deserialize a nil object
  void unpack();

  //! Deserialize any supported object
  template<typename T> T unpack();

private:
  //! Insert data into the message buffer
  template<typename T> message& insert(T data);

  //! Extract data from the message buffer without advancing position
  template<typename T> T extract();

  std::vector<uint8_t> m_data;           //!< message buffer
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
  constexpr bool predicate =
      std::numeric_limits<T>::is_integer && !std::numeric_limits<T>::is_signed;
  static_assert(predicate,
      "data being inserted should be of an unsigned integer type");

  const uint8_t* first = reinterpret_cast<const uint8_t*>(&data);
  const uint8_t* last = first + sizeof(data);

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
  constexpr bool predicate =
      std::is_same<uint8_t, std::underlying_type<format>::type>::value;
  static_assert(predicate,
      "underlying type of the format enum class is not uint8_t");

  insert(static_cast<uint8_t>(data));

  return *this;
}

template<typename T>
T message::extract()
{
  constexpr bool predicate =
      std::numeric_limits<T>::is_integer && !std::numeric_limits<T>::is_signed;
  static_assert(predicate,
      "type being extracted should be an unsigned integer");

  T data;

  if (m_data.size() - m_pos < sizeof(data))
    throw unpack_error(__func__, "message size insufficient");

  uint8_t* result = reinterpret_cast<uint8_t*>(&data);

  std::copy_n(&m_data[m_pos], sizeof(data), result);

  return data;
}

template<>
uint8_t message::extract<uint8_t>()
{
  if (m_data.size() - m_pos < 1)
    throw unpack_error(__func__, "message size insufficient");

  return m_data[m_pos];
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
    insert(value);
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
    insert(htobe64(reinterpret_cast<uint64_t&>(value)));
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
    insert(htobe32(reinterpret_cast<uint32_t&>(value)));
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
    insert(htobe16(reinterpret_cast<uint16_t&>(value)));
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
    insert(reinterpret_cast<uint8_t&>(value));
  }
  else
  {
    insert(reinterpret_cast<uint8_t&>(value));
  }
  return *this;
}

message& message::pack(float value)
{
  constexpr bool predicate = std::numeric_limits<float>::is_iec559;
  static_assert(predicate,
      "The single precision floating point type is not IEEE 754 format");

  insert(format::float32);
  insert(htobe32(reinterpret_cast<const uint32_t&>(value)));
  return *this;
}

message& message::pack(double value)
{
  constexpr bool predicate = std::numeric_limits<double>::is_iec559;
  static_assert(predicate,
      "The double precision floating point type is not IEEE 754 format");

  insert(format::float64);
  insert(htobe64(reinterpret_cast<const uint64_t&>(value)));
  return *this;
}

void message::unpack()
{
  if (extract<uint8_t>() != format::nil)
    throw unpack_error(__func__, "message object is not nil");
  m_pos++;
}

template<typename T>
T message::unpack()
{
  // Should not compile for unsupported types
}

template<>
bool message::unpack<bool>()
{
  bool value;
  uint8_t fmt = extract<uint8_t>();

  if (fmt == format::boolfalse)
    value = false;
  else if (fmt == format::booltrue)
    value = true;
  else
    throw unpack_error(__func__, "message object is not boolean");

  m_pos++;

  return value;
}

template<>
uint64_t message::unpack<uint64_t>()
{
  uint8_t fmt = extract<uint8_t>();

  if (fmt < (1 << 7))
  {
    m_pos++;
    return fmt;
  }

  uint64_t data;

  if (fmt == format::uint8)
  {
    m_pos++;
    data = extract<uint8_t>();
    m_pos += sizeof(uint8_t);
  }
  else if (fmt == format::uint16)
  {
    m_pos++;
    data = be16toh(extract<uint16_t>());
    m_pos += sizeof(uint16_t);
  }
  else if (fmt == format::uint32)
  {
    m_pos++;
    data = be32toh(extract<uint32_t>());
    m_pos += sizeof(uint32_t);
  }
  else if (fmt == format::uint64)
  {
    m_pos++;
    data = be64toh(extract<uint64_t>());
    m_pos += sizeof(uint64_t);
  }
  else
  {
    throw unpack_error(__func__, "message object is not an unsigned integer");
  }

  return data;
}

} /* namespace comm */

#endif /* INCLUDE_MESSAGE_HPP_ */
