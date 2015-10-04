/*
 * Copyright 2015 C. Brett Witherspoon
 */

#include <cstring>
#include "comm/endian.hpp"

namespace comm
{
int64_t betoh(int64_t val)
{
  uint64_t dat;
  std::memcpy(&dat, &val, sizeof(val));
  dat = be64toh(dat);
  return static_cast<int64_t>(dat);
}

int32_t betoh(int32_t val)
{
  uint32_t dat;
  std::memcpy(&dat, &val, sizeof(val));
  dat = be32toh(dat);
  return static_cast<int32_t>(dat);
}

int16_t betoh(int16_t val)
{
  uint16_t dat;
  std::memcpy(&dat, &val, sizeof(val));
  dat = be16toh(dat);
  return static_cast<int16_t>(dat);
}

double betoh(double val)
{
  uint64_t dat;
  std::memcpy(&dat, &val, sizeof(val));
  dat = be64toh(dat);
  std::memcpy(&val, &dat, sizeof(dat));
  return val;
}

float betoh(float val)
{
  uint32_t dat;
  std::memcpy(&dat, &val, sizeof(val));
  dat = be32toh(dat);
  std::memcpy(&val, &dat, sizeof(dat));
  return val;
}

int64_t htobe(int64_t val)
{
  uint64_t dat;
  std::memcpy(&dat, &val, sizeof(val));
  dat = htobe64(dat);
  return static_cast<int64_t>(dat);
}

int32_t htobe(int32_t val)
{
  uint32_t dat;
  std::memcpy(&dat, &val, sizeof(val));
  dat = htobe32(dat);
  return static_cast<int32_t>(dat);
}

int16_t htobe(int16_t val)
{
  uint16_t dat;
  std::memcpy(&dat, &val, sizeof(val));
  dat = htobe16(dat);
  return static_cast<int16_t>(dat);
}

double htobe(double val)
{
  uint64_t dat;
  std::memcpy(&dat, &val, sizeof(val));
  dat = htobe64(dat);
  std::memcpy(&val, &dat, sizeof(dat));
  return val;
}

float htobe(float val)
{
  uint32_t dat;
  std::memcpy(&dat, &val, sizeof(val));
  dat = htobe32(dat);
  std::memcpy(&val, &dat, sizeof(dat));
  return val;
}
} /* namespace comm */
