/*
 * Copyright 2015 C. Brett Witherspoon
 */

#include <limits>
#include <type_traits>

#include "comm/endian.hpp"
#include "comm/message.hpp"

namespace comm {

message::message() :
    m_data(), m_pos(0)
{ }

message::message(message&& other) :
    m_data(std::move(other.m_data)),
    m_pos(other.m_pos)
{ }

void message::clear()
{
  m_data.clear();
  m_pos = 0;
}

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

message& message::deserialize()
{
  if (extract() != formats::nil)
    throw deserialize_error(__func__, "failed to deserialize a nil object");

  m_pos++;

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

message& message::operator=(message&& other)
{
  m_data = std::move(other.m_data);
  m_pos = other.m_pos;
  return *this;
}

void message::insert(uint8_t data)
{
  m_data.push_back(data);
}

uint8_t message::extract()
{
  if (m_data.size() - m_pos < 1)
    throw deserialize_error(__func__, "message size insufficient");

  return m_data[m_pos];
}

void message::insert(formats data)
{
  constexpr bool predicate =
      std::is_same<uint8_t, std::underlying_type<formats>::type>::value;
  static_assert(predicate,
      "underlying type of the format enum class is not uint8_t");

  insert(static_cast<uint8_t>(data));
}

message::deserialize_error::deserialize_error(
    const std::string& loc, const std::string& msg)
        : std::runtime_error(loc + ": " + msg)
{ }

message::deserialize_error::deserialize_error(
    const char* loc, const char* msg)
        : deserialize_error(std::string(loc), std::string(msg))
{ }

} /* namespace comm */
