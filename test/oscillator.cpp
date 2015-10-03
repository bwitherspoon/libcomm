/*
 * Copyright 2015 C. Brett Witherspoon
 */

#define BOOST_TEST_MODULE comm_tests
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <cmath>

#include "comm/oscillator.hpp"

BOOST_AUTO_TEST_CASE(double_oscillator_test)
{
  const double thresh = 1e-14;
  const double freq = 10;
  const double rate = 50;

  std::array<double, 50> output;

  comm::oscillator<double> osc(freq, rate);

  osc(output);

  for (auto i = 0U; i < output.size(); ++i)
    if (std::abs(output[i]) > thresh)
      BOOST_CHECK_CLOSE(output[i], sin(2*M_PI*freq/rate*(i+1)), 0.0001);
}

BOOST_AUTO_TEST_CASE(complex_oscillator_test)
{
  const double thresh = 1e-12;
  const double freq = 100;
  const double rate = 650;

  std::array<std::complex<double>, 650> output;

  comm::oscillator<std::complex<double>> osc(freq, rate);

  osc(output);

  for (auto i = 0U; i < output.size(); ++i)
  {
    if (std::abs(output[i].real()) > thresh)
      BOOST_CHECK_CLOSE(output[i].real(), cos(2*M_PI*freq/rate*i), 0.0001);
    if (std::abs(output[i].imag()) > thresh)
      BOOST_CHECK_CLOSE(output[i].imag(), sin(2*M_PI*freq/rate*i), 0.01);
  }
}
