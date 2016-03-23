/*
 * Copyright 2015 C. Brett Witherspoon
 */

#define BOOST_TEST_MODULE signum_tests
#include <boost/test/unit_test.hpp>

#include <cmath>
#include <cstdint>
#include <limits>
#include <type_traits>

#include "signum/fixed.hpp"

namespace
{
template<typename Float, typename Fixed>
void _fixed_to_floating_test()
{
  using signum::fixed_to_floating;

  const auto delta = 1.0 / (std::numeric_limits<Fixed>::max() + 1.0);

  const auto fixed_lower  = std::numeric_limits<Fixed>::lowest();
  const auto float_lower  = (std::is_signed<Fixed>::value) ? -1.0 : 0.0;

  const auto fixed_median = std::numeric_limits<Fixed>::max() / 2;
  const auto float_median = 0.5 - delta;

  const auto fixed_upper  = std::numeric_limits<Fixed>::max();
  const auto float_upper  = 1.0 - delta;

  const auto lower  = fixed_to_floating<Float, Fixed>(fixed_lower);
  const auto median = fixed_to_floating<Float, Fixed>(fixed_median);
  const auto upper  = fixed_to_floating<Float, Fixed>(fixed_upper);

  BOOST_CHECK_CLOSE(lower,
                    static_cast<Float>(float_lower),
                    static_cast<Float>(100.0*delta/float_lower));
  BOOST_CHECK_CLOSE(median,
                    static_cast<Float>(float_median),
                    static_cast<Float>(100.0*delta/float_median));
  BOOST_CHECK_CLOSE(upper,
                    static_cast<Float>(float_upper),
                    static_cast<Float>(100.0*delta/float_upper));
}

template<typename Fixed, typename Float>
void _floating_to_fixed_test()
{
  using signum::floating_to_fixed;

  const auto delta = 1.0 / (std::numeric_limits<Fixed>::max() + 1.0);
  const auto epsilon = std::numeric_limits<Float>::epsilon();

  const auto fixed_lower  = std::numeric_limits<Fixed>::lowest();
  const auto float_lower  = (std::is_signed<Fixed>::value) ? -1.0 : 0.0;

  const auto fixed_median = std::numeric_limits<Fixed>::max() / 2;
  const auto float_median = 0.5 - epsilon - delta;

  const auto fixed_upper  = std::numeric_limits<Fixed>::max();
  const auto float_upper  = 1.0 - epsilon - delta;

  const auto lower  = floating_to_fixed<Fixed, Float>(float_lower);
  const auto median = floating_to_fixed<Fixed, Float>(float_median);
  const auto upper  = floating_to_fixed<Fixed, Float>(float_upper);

  BOOST_CHECK_EQUAL(lower, fixed_lower);
  BOOST_CHECK_EQUAL(median, fixed_median);
  BOOST_CHECK_EQUAL(upper, fixed_upper);
}
} // namespace (anonymous)

BOOST_AUTO_TEST_CASE(fixed_to_floating_unsigned_single_test)
{
  _fixed_to_floating_test<float, uint8_t>();
  _fixed_to_floating_test<float, uint16_t>();
  _fixed_to_floating_test<float, uint32_t>();
  _fixed_to_floating_test<float, uint64_t>();
}

BOOST_AUTO_TEST_CASE(fixed_to_floating_signed_single_test)
{
  _fixed_to_floating_test<float, int8_t>();
  _fixed_to_floating_test<float, int16_t>();
  _fixed_to_floating_test<float, int32_t>();
  _fixed_to_floating_test<float, int64_t>();
}

BOOST_AUTO_TEST_CASE(fixed_to_floating_unsigned_double_test)
{
  _fixed_to_floating_test<double, uint8_t>();
  _fixed_to_floating_test<double, uint16_t>();
  _fixed_to_floating_test<double, uint32_t>();
  _fixed_to_floating_test<double, uint64_t>();
}

BOOST_AUTO_TEST_CASE(fixed_to_floating_signed_double_test)
{
  _fixed_to_floating_test<double, int8_t>();
  _fixed_to_floating_test<double, int16_t>();
  _fixed_to_floating_test<double, int32_t>();
  _fixed_to_floating_test<double, int64_t>();
}

BOOST_AUTO_TEST_CASE(floating_to_fixed_unsigned_single_test)
{
  _floating_to_fixed_test<uint8_t, float>();
  _floating_to_fixed_test<uint16_t, float>();
}

BOOST_AUTO_TEST_CASE(floating_to_fixed_signed_single_test)
{
  _floating_to_fixed_test<int8_t, float>();
  _floating_to_fixed_test<int16_t, float>();
}

BOOST_AUTO_TEST_CASE(floating_to_fixed_unsigned_double_test)
{
  _floating_to_fixed_test<uint8_t, double>();
  _floating_to_fixed_test<uint16_t, double>();
  _floating_to_fixed_test<uint32_t, double>();
}

BOOST_AUTO_TEST_CASE(floating_to_fixed_signed_double_test)
{
  _floating_to_fixed_test<int8_t, double>();
  _floating_to_fixed_test<int16_t, double>();
  _floating_to_fixed_test<int32_t, double>();
}

