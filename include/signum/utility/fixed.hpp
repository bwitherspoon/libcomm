/*
 * Copyright 2015 C. Brett Witherspoon
 */

#ifndef SIGNUM_FIXED_HPP_
#define SIGNUM_FIXED_HPP_

#include <cmath>
#include <limits>
#include <type_traits>

namespace signum
{
namespace utility
{
//! Convert a fixed-point type to a floating-point type
template<typename Float, typename Fixed>
Float fixed_to_float(Fixed fixed)
{
  static_assert(
    std::is_floating_point<Float>::value && std::is_integral<Fixed>::value,
    "Template arguments must be floating-point and integral types");

  const auto scale = 1.0 / (std::numeric_limits<Fixed>::max() + 1.0);

  return static_cast<Float>(scale * fixed);
}

//! Convert a floating-point type to a fixed-point type
template<typename Fixed, typename Float>
Fixed float_to_fixed(Float floating)
{
  static_assert(
    std::is_floating_point<Float>::value && std::is_integral<Fixed>::value,
    "Template arguments must be floating-point and integral types");
  static_assert(
    sizeof(Fixed) < sizeof(Float),
    "An integral type larger then the floating-point type is not supported");

  const auto min = std::numeric_limits<Fixed>::min();
  const auto max = std::numeric_limits<Fixed>::max();

  switch (std::fpclassify(floating))
  {
    case FP_NAN:
    case FP_ZERO:
    case FP_SUBNORMAL:
        return static_cast<Fixed>(0);
    case FP_INFINITE:
        return (std::signbit(floating)) ? min : max;
    default:
        auto ret = floating * (std::numeric_limits<Fixed>::max() + 1.0);
        ret = std::fmin(ret, max);
        ret = std::fmax(ret, min);
        return static_cast<Fixed>(std::lround(ret));
  }
}
} /* namespace utility */
} /* namespace signum */

#endif /* SIGNUM_FIXED_HPP_ */
