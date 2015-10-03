/*
 * Copyright 2015 C. Brett Witherspoon
 */

#ifndef INCLUDE_MESSAGE_HPP_
#define INCLUDE_MESSAGE_HPP_

#include <endian.h>
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace comm
{

/**
 * \brief A class for serializing and deserializing messages
 */
class message
{
  friend class socket;
public:
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
    explicit deserialize_error(const std::string& loc, const std::string& msg) :
        std::runtime_error(loc + ": " + msg)
    {
    }
    explicit deserialize_error(const char* loc, const char* msg) :
        deserialize_error(std::string(loc), std::string(msg))
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
  template<typename T> message& operator<<(T value) { return serialize(value); }

  //! Deserialize a nil type
  message& deserialize();

  //! Deserialize any supported type
  template<typename T> message& deserialize(T& value);

  //! Deserialize any supported type
  template<typename T>
  message& operator>>(T& value) { return deserialize(value); }

private:
  static uint64_t betoh(uint64_t val)
  {
    return be64toh(val);
  }

  static uint32_t betoh(uint32_t val)
  {
    return be32toh(val);
  }

  static uint16_t betoh(uint16_t val)
  {
    return be16toh(val);
  }

  static int64_t betoh(int64_t val)
  {
    uint64_t dat;
    std::memcpy(&dat, &val, sizeof(val));
    dat = be64toh(dat);
    return static_cast<int64_t>(dat);
  }


  static int32_t betoh(int32_t val)
  {
    uint32_t dat;
    std::memcpy(&dat, &val, sizeof(val));
    dat = be32toh(dat);
    return static_cast<int32_t>(dat);
  }

  static int16_t betoh(int16_t val)
  {
    uint16_t dat;
    std::memcpy(&dat, &val, sizeof(val));
    dat = be16toh(dat);
    return static_cast<int16_t>(dat);
  }

  static double betoh(double val)
  {
    uint64_t dat;
    std::memcpy(&dat, &val, sizeof(val));
    dat = be64toh(dat);
    std::memcpy(&val, &dat, sizeof(dat));
    return val;
  }

  static float betoh(float val)
  {
    uint32_t dat;
    std::memcpy(&dat, &val, sizeof(val));
    dat = be32toh(dat);
    std::memcpy(&val, &dat, sizeof(dat));
    return val;
  }

  static uint64_t htobe(uint64_t val)
  {
    return htobe64(val);
  }

  static uint32_t htobe(uint32_t val)
  {
    return htobe32(val);
  }

  static uint16_t htobe(uint16_t val)
  {
    return htobe16(val);
  }

  static int64_t htobe(int64_t val)
  {
    uint64_t dat;
    std::memcpy(&dat, &val, sizeof(val));
    dat = htobe64(dat);
    return static_cast<int64_t>(dat);
  }

  static int32_t htobe(int32_t val)
  {
    uint32_t dat;
    std::memcpy(&dat, &val, sizeof(val));
    dat = htobe32(dat);
    return static_cast<int32_t>(dat);
  }

  static int16_t htobe(int16_t val)
  {
    uint16_t dat;
    std::memcpy(&dat, &val, sizeof(val));
    dat = htobe16(dat);
    return static_cast<int16_t>(dat);
  }

  static double htobe(double val)
  {
    uint64_t dat;
    std::memcpy(&dat, &val, sizeof(val));
    dat = htobe64(dat);
    std::memcpy(&val, &dat, sizeof(dat));
    return val;
  }

  static float htobe(float val)
  {
    uint32_t dat;
    std::memcpy(&dat, &val, sizeof(val));
    dat = htobe32(dat);
    std::memcpy(&val, &dat, sizeof(dat));
    return val;
  }

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

////////////////////////////////////////////////////////////////////////////////

void message::insert(uint8_t data)
{
  m_data.push_back(data);
}

void message::insert(formats data)
{
  constexpr bool predicate =
      std::is_same<uint8_t, std::underlying_type<formats>::type>::value;
  static_assert(predicate,
      "underlying type of the format enum class is not uint8_t");

  insert(static_cast<uint8_t>(data));
}

template<typename T>
void message::insert(const T& data)
{
  const uint8_t* first = reinterpret_cast<const uint8_t*>(&data);

  m_data.insert(std::end(m_data), first, first + sizeof(data));
}

uint8_t message::extract()
{
  if (m_data.size() - m_pos < 1)
    throw deserialize_error(__func__, "message size insufficient");

  return m_data[m_pos];
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

////////////////////////////////////////////////////////////////////////////////

message& message::serialize()
{
  insert(formats::nil);
  return *this;
}

message& message::serialize(bool value)
{
  insert((value) ? formats::booltrue : formats::boolfalse);
  return *this;
}

message& message::serialize(uint64_t value)
{
  if (value > std::numeric_limits<uint32_t>::max())
  {
    insert(formats::uint64);
    insert(htobe(value));
  }
  else
  {
    serialize(static_cast<uint32_t>(value));
  }
  return *this;
}

message& message::serialize(uint32_t value)
{
  if (value > std::numeric_limits<uint16_t>::max())
  {
    insert(formats::uint32);
    insert(htobe(value));
  }
  else
  {
    serialize(static_cast<uint16_t>(value));
  }
  return *this;
}

message& message::serialize(uint16_t value)
{
  if (value > std::numeric_limits<uint8_t>::max())
  {
    insert(formats::uint16);
    insert(htobe(value));
  }
  else
  {
    serialize(static_cast<uint8_t>(value));
  }
  return *this;
}

message& message::serialize(uint8_t value)
{
  if (value < (1 << 7))
  {
    insert(value);
  }
  else
  {
    insert(formats::uint8);
    insert(value);
  }
  return *this;
}

message& message::serialize(int64_t value)
{
  constexpr int32_t min = std::numeric_limits<int32_t>::min();
  constexpr int32_t max = std::numeric_limits<int32_t>::max();

  if (value < min or value > max)
  {
    insert(formats::int64);
    insert(htobe(value));
  }
  else
  {
    serialize(static_cast<int32_t>(value));
  }

  return *this;
}

message& message::serialize(int32_t value)
{
  constexpr int16_t min = std::numeric_limits<int16_t>::min();
  constexpr int16_t max = std::numeric_limits<int16_t>::max();

  if (value < min or value > max)
  {
    insert(formats::int32);
    insert(htobe(value));
  }
  else
  {
    serialize(static_cast<int16_t>(value));
  }

  return *this;
}

message& message::serialize(int16_t value)
{
  constexpr int8_t min = std::numeric_limits<int8_t>::min();
  constexpr int8_t max = std::numeric_limits<int8_t>::max();

  if (value < min or value > max)
  {
    insert(formats::int16);
    insert(htobe(value));
  }
  else
  {
    serialize(static_cast<int8_t>(value));
  }

  return *this;
}

message& message::serialize(int8_t value)
{
  if (value < -(1 << 5))
  {
    insert(formats::int8);
    insert(value);
  }
  else
  {
    insert(value);
  }
  return *this;
}

message& message::serialize(float value)
{
  constexpr bool predicate = std::numeric_limits<float>::is_iec559;
  static_assert(predicate,
      "native single precision floating point type is not IEEE 754 format");

  insert(formats::float32);
  insert(htobe(value));

  return *this;
}

message& message::serialize(double value)
{
  constexpr bool predicate = std::numeric_limits<double>::is_iec559;
  static_assert(predicate,
      "native double precision floating point type is not IEEE 754 format");

  insert(formats::float64);
  insert(htobe(value));

  return *this;
}

////////////////////////////////////////////////////////////////////////////////

uint64_t message::deserialize_format(uint64_t value, uint8_t format)
{
  uint64_t result;

  if (format == formats::uint64)
  {
    result = betoh(extract(value));
    m_pos += sizeof(value);
  }
  else
  {
    result = deserialize_format(static_cast<uint32_t>(value), format);
  }

  return result;
}

uint32_t message::deserialize_format(uint32_t value, uint8_t format)
{
  uint32_t result;

  if (format == formats::uint32)
  {
    result = betoh(extract(value));
    m_pos += sizeof(value);
  }
  else
  {
    result = deserialize_format(static_cast<uint16_t>(value), format);
  }

  return result;
}

uint16_t message::deserialize_format(uint16_t value, uint8_t format)
{
  uint16_t result;

  if (format == formats::uint16)
  {
    result = betoh(extract(value));
    m_pos += sizeof(value);
  }
  else
  {
    result = deserialize_format(static_cast<uint8_t>(value), format);
  }

  return result;
}

uint8_t message::deserialize_format(uint8_t value, uint8_t format)
{
  uint8_t result;

  if (format == formats::uint8)
  {
    result = extract(value);
    m_pos += sizeof(value);
  }
  else if (format < (1 << 7)) // fixint
  {
    result = format;
  }
  else
  {
    throw deserialize_error(__func__, "failed to deserialize a positive integer object");
  }

  return result;
}

int64_t message::deserialize_format(int64_t value, uint8_t format)
{
  int64_t result;

  if (format == formats::int64)
  {
    result = betoh(extract(value));
    m_pos += sizeof(value);
  }
  else
  {
    result = deserialize_format(static_cast<int32_t>(value), format);
  }

  return result;
}

int32_t message::deserialize_format(int32_t value, uint8_t format)
{
  int32_t result;

  if (format == formats::int32)
  {
    result = betoh(extract(value));
    m_pos += sizeof(value);
  }
  else
  {
    result = deserialize_format(static_cast<int16_t>(value), format);
  }

  return result;
}

int16_t message::deserialize_format(int16_t value, uint8_t format)
{
  int16_t result;

  if (format == formats::int16)
  {
    result = betoh(extract(value));
    m_pos += sizeof(value);
  }
  else
  {
    result = deserialize_format(static_cast<int8_t>(value), format);
  }

  return result;
}

int8_t message::deserialize_format(int8_t value, uint8_t format)
{
  int8_t result;

  if (format == formats::int8)
  {
    result = extract(value);
    m_pos += sizeof(value);
  }
  else if ((format & formats::negfixint) == formats::negfixint) // fixint
  {
    result = reinterpret_cast<int8_t&>(format);
  }
  else
  {
    throw deserialize_error(__func__, "failed to deserialize a negative integer object");
  }

  return result;
}

double message::deserialize_format(double value, uint8_t format)
{
  double result;

  if (format == formats::float64)
  {
    result = betoh(extract(value));
    m_pos += sizeof(value);
  }
  else
  {
    result = deserialize_format(static_cast<float>(value), format);
  }

  return result;
}

float message::deserialize_format(float value, uint8_t format)
{
  float result;

  if (format == formats::float32)
  {
    result = betoh(extract(value));
    m_pos += sizeof(value);
  }
  else
  {
    throw deserialize_error(__func__, "failed to deserialize a floating point object");
  }

  return result;
}

////////////////////////////////////////////////////////////////////////////////

message& message::deserialize()
{
  if (extract() != formats::nil)
    throw deserialize_error(__func__, "failed to deserialize a nil object");

  m_pos++;

  return *this;
}

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
message& message::deserialize<bool>(bool& value)
{
  uint8_t format = extract();

  if (format == formats::boolfalse)
    value = false;
  else if (format == formats::booltrue)
    value = true;
  else
    throw deserialize_error(__func__, "failed to deserialize a boolean object");

  m_pos++;

  return *this;
}

} /* namespace comm */

#endif /* INCLUDE_MESSAGE_HPP_ */
