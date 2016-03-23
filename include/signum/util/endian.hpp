/*
 * Copyright 2015 C. Brett Witherspoon
 */

#ifndef SIGNUM_ENDIAN_HPP_
#define SIGNUM_ENDIAN_HPP_

#include <endian.h>
#include <cstdint>

namespace signum
{
namespace util
{
//! Convert big endian to host byte order
inline uint64_t betoh(uint64_t val) { return be64toh(val); }

//! Convert big endian to host byte order
inline uint32_t betoh(uint32_t val) { return be32toh(val); }

//! Convert big endian to host byte order
inline uint16_t betoh(uint16_t val) { return be16toh(val); }

//! Convert big endian to host byte order
int64_t betoh(int64_t val);

//! Convert big endian to host byte order
int32_t betoh(int32_t val);

//! Convert big endian to host byte order
int16_t betoh(int16_t val);

//! Convert big endian to host byte order
double betoh(double val);

//! Convert big endian to host byte order
float betoh(float val);

//! Convert host to big endian byte order
inline uint64_t htobe(uint64_t val) { return htobe64(val); }

//! Convert host to big endian byte order
inline uint32_t htobe(uint32_t val) { return htobe32(val); }

//! Convert host to big endian byte order
inline uint16_t htobe(uint16_t val) { return htobe16(val); }

//! Convert host to big endian byte order
int64_t htobe(int64_t val);

//! Convert host to big endian byte order
int32_t htobe(int32_t val);

//! Convert host to big endian byte order
int16_t htobe(int16_t val);

//! Convert host to big endian byte order
double htobe(double val);

//! Convert host to big endian byte order
float htobe(float val);
} /* namespace util */
} /* namespace signum */

#endif /* SIGNUM_ENDIAN_HPP_ */
