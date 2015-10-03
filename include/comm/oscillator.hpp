/*
 * Copyright 2015 C. Brett Witherspoon
 */

#ifndef OSCILLATOR_HPP_
#define OSCILLATOR_HPP_

#include <complex>

namespace comm {

template<class T>
class oscillator
{
public:
  using sample_type = T;

  oscillator(double frequency, double sample_rate, double amplitude=1)
    : m_freq(frequency),
      m_rate(sample_rate),
      m_ampl(amplitude)
  {
    static_assert(std::is_floating_point<sample_type>::value,
                  "Sample type must be a floating point type");

    m_y1 = 0;
    m_y2 = -m_ampl * sin(2 * M_PI * m_freq / m_rate);
    m_2cos_omega  = 2 * cos(2 * M_PI * m_freq / m_rate);
  }

  template<size_t N>
  void operator()(std::array<sample_type, N> &output)
  {
    for (auto &y0 : output)
    {
      y0 = m_2cos_omega * m_y1 - m_y2;
      m_y2 = m_y1;
      m_y1 = y0;
    }
  }

  double frequency() { return m_freq; }

  double sample_rate() { return m_rate; }

  double amplitude() { return m_ampl; }

private:
  double m_freq;
  double m_rate;
  double m_ampl;

  double m_y1;
  double m_y2;
  double m_2cos_omega;
};

template<>
template<class T>
class oscillator<std::complex<T>>
{
public:
  using sample_type = std::complex<T>;

  oscillator(double frequency, double sample_rate, double amplitude = 1)
    : m_freq(frequency),
      m_rate(sample_rate),
      m_ampl(amplitude)
  {
    static_assert(std::is_floating_point<T>::value,
                  "Sample type must be a floating point type");

    m_cos_omega = cos(2 * M_PI * m_freq / m_rate);
    m_sin_omega = sin(2 * M_PI * m_freq / m_rate);
    m_cos1 = m_ampl * m_cos_omega;
    m_sin1 = -m_ampl * m_sin_omega;
  }

  template<size_t N>
  void operator()(std::array<sample_type, N> &output)
  {
    T cos0;
    T sin0;

    for (auto &out : output)
    {
      cos0 = m_cos_omega * m_cos1 - m_sin_omega * m_sin1;
      sin0 = m_sin_omega * m_cos1 + m_cos_omega * m_sin1;
      m_cos1 = cos0;
      m_sin1 = sin0;
      out = std::complex<T>(cos0, sin0);
    }
  }

  double frequency() { return m_freq; }

  double sample_rate() { return m_rate; }

  double amplitude() { return m_ampl; }

private:
  double m_freq;
  double m_rate;
  double m_ampl;

  double m_cos1;
  double m_sin1;
  double m_cos_omega;
  double m_sin_omega;
};

} /* namespace comm */

#endif /* OSCILLATOR_HPP_ */
