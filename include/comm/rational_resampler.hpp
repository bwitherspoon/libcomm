/*
 * Copyright 2015 C. Brett Witherspoon
 */

#ifndef RATIONAL_RESAMPLER_HPP_
#define RATIONAL_RESAMPLER_HPP_

#include <array>
#include <algorithm>
#include <complex>

namespace comm {
/**
 *  A rational resampler
 */
template<unsigned int I,
         unsigned int D,
         size_t M,
         typename T = float,
         typename U = std::complex<float>>
class rational_resampler
{
public:
  using coeffs_type = T;
  using sample_type = U;

  static constexpr unsigned int interpolation() { return I; }

  static constexpr unsigned int decimation() { return D; }

  static constexpr size_t filter_size() { return M / I; }

  static constexpr size_t output_size(size_t input_size)
  {
    return ((input_size*I) % D) ? (input_size*I) / D + 1 : (input_size*I) / D;
  }

  /**
   * Construct a polyphase filter bank rational resampler from a prototype filter
   * @param prototype - a prototype filter
   */
  explicit rational_resampler(const std::array<coeffs_type, M>& prototype);

  /**
   * Execute the resampler
   * @param input - input sample array
   * @param output - output sample array
   */
  template<size_t N>
  void operator()(const std::array<sample_type, N> &input,
                  std::array<sample_type, output_size(N)> &output);

private:
  alignas(16) std::array<std::array<coeffs_type, filter_size()>, interpolation()> m_filters;
  alignas(16) std::array<sample_type, filter_size()> m_delay;
  unsigned int m_counter;
  unsigned int m_offset;
};

/**
 * Construct a polyphase filter bank rational resampler from a prototype filter
 * @param prototype - a prototype filter
 */
rational_resampler::rational_resampler(const std::array<coeffs_type, M>& prototype)
  : m_counter(0), m_offset(~0)
{
  static_assert((M % I == 0), "Length of prototype filter must be evenly divisible by interpolation.");
  static_assert((I != 0 && D != 0), "Interpolation and/or decimation cannot be zero.");

  m_delay.fill(0);

  for (auto i = 0U; i < m_filters.size(); ++i)
    for (auto j = 0U; j < filter_size(); ++j)
      m_filters[i][j] = prototype[i + j*filter_size()];
}

/**
 * Execute the resampler
 * @param input - input sample array
 * @param output - output sample array
 */
template<size_t N>
void operator()(const std::array<sample_type, N> &input,
                std::array<sample_type, output_size(N)> &output)
{
  unsigned int n = 0;

  for (auto m = 0U; m < output.size(); ++m)
  {
    // update delay line
    unsigned int offset = (m_counter * decimation()) / interpolation();
    if (offset != m_offset)
    {
      std::copy_backward(m_delay.begin(), m_delay.end()-1, m_delay.end());
      m_delay[0] = input[n];
      n++;
    }
    m_offset = offset;

    // select the subfilter index
    unsigned int filt = (m_counter * decimation()) % interpolation();

    // filter
    output[m] = 0;
    for (auto i = 0U; i < m_delay.size(); ++i)
      output[m] += m_filters[filt][i] * m_delay[i];

    m_counter++;
  }

  m_counter %= interpolation();
}

/**
 * Helper function to create a resampler using function template deduction
 * @param prototype - a prototype filter
 * @return
 */
template<unsigned int I,
         unsigned int D,
         typename U = std::complex<float>,
         size_t M,
         typename T>
rational_resampler<I, D, M, T, U>
make_rational_resampler(const std::array<T, M> &prototype)
{
  return rational_resampler<I, D, M, T, U>(prototype);
}

} /* namespace comm */

#endif /* RATIONAL_RESAMPLER_HPP_ */
