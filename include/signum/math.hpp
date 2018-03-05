/*
 * Copyright 2015-2018 C. Brett Witherspoon
 *
 * This file is part of the signum library
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef SIGNUM_MATH_HPP_
#define SIGNUM_MATH_HPP_

#include <type_traits>

namespace signum
{
namespace math
{

/**
 * \brief Return the absolute value of the argument.
 *
 * Until C++14 the STL version of this function is not constexpr, so we use
 * this.
 *
 * \param v the argument
 * \return the absolute value of v
 *
 */
template<typename T>
constexpr T abs(T v)
{
  static_assert(std::is_arithmetic<T>::value,
      "template argument must be an arithmetic type.");

  return (v < 0) ? -v : v;
}

/**
 * \brief Return the maximum of the arguments
 *
 * Until C++14 the STL version of this function is not constexpr, so we use
 * this.
 *
 * \param a an argument
 * \param b an argument
 * \return the maximum of a and b
 */
template<typename T>
constexpr T max(T a, T b)
{
  static_assert(std::is_arithmetic<T>::value,
      "template argument must be an arithmetic type.");

  return (a > b) ? a : b;
}

/**
 * \brief Compute the greatest common divisor of the arguments
 * \param a an integer
 * \param b an integer
 * \return the greatest common divisor of a and b
 */
template<typename T>
constexpr T gcd(T a, T b)
{
  static_assert(std::is_integral<T>::value,
      "template argument must be an integral type.");

  return (b == 0) ? a : gcd(b, a % b);
}

/**
 * \brief Compute the least common multiple of the aruments
 * \param a an integer
 * \param b an integer
 * \return the least common multiple of a and b
 */
template <typename T>
constexpr T lcm(T a, T b)
{
  return (a == 0 and b == 0) ? 0 : (abs(a) / gcd(a, b)) * abs(b);
}

/**
 * \brief Compute the next higher power of two
 * \param a an integer
 * \return the smallest power of two greater then a
 */
template <typename T>
constexpr T nextpow2(T a)
{
  static_assert(std::is_integral<T>::value,
      "template argument must be an integral type.");

  T n = 1;
  while (n < a) n <<= 1;
  return n;
}

/**
 * \brief Determine if an integral value is a power of two
 * \param a an integral value
 * \return true if the integral value is a power of two or false otherwise
 */
template <typename T>
constexpr bool ispow2(T x)
{
  static_assert(std::is_integral<T>::value,
      "template argument must be an integral type.");

  return !(x == 0 || (x & (x - 1)));
}

} /* namespace math */
} /* namespace signum */
#endif /* SIGNUM_MATH_HPP_ */
