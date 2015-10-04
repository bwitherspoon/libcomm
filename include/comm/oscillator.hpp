/*
 * Copyright 2015 C. Brett Witherspoon
 */

#ifndef OSCILLATOR_HPP_
#define OSCILLATOR_HPP_

#include <complex>

namespace comm
{
//! An oscillator with real output
template<typename T>
class oscillator
{
public:
  using sample_type = T;

  //! Construct an oscillator with real output
  oscillator(T frequency, T sample_rate, T amplitude = 1);

  //! Execute the oscillator
  template<typename U> void operator()(U& output);

  //! Returns the frequency of the oscillator
  T frequency() { return m_freq; }

  //! Returns the sample rate of the oscillator
  T sample_rate() { return m_rate; }

  //! Returns the amplitude of the oscillator
  T amplitude() { return m_ampl; }

private:
  T m_freq;
  T m_rate;
  T m_ampl;
  T m_out1;
  T m_out2;
  T m_2cos_omega;
};

///////////////////////////////////////////////////////////////////////////////

template<typename T>
oscillator<T>::oscillator(T frequency, T sample_rate, T amplitude)
  : m_freq(frequency),
    m_rate(sample_rate),
    m_ampl(amplitude)
{
  static_assert(std::is_floating_point<sample_type>::value,
    "Sample type must be a floating point type");

  m_out1 = 0;
  m_out2 = -m_ampl * sin(2 * M_PI * m_freq / m_rate);
  m_2cos_omega  = 2 * cos(2 * M_PI * m_freq / m_rate);
}

template<typename T>
template<typename U>
void oscillator<T>::operator()(U& output)
{
  for (auto &out0 : output)
  {
    out0 = m_2cos_omega * m_out1 - m_out2;
    m_out2 = m_out1;
    m_out1 = out0;
  }
}

///////////////////////////////////////////////////////////////////////////////

//! An oscillator with complex output
template<typename T>
class oscillator<std::complex<T>>
{
public:
  using sample_type = std::complex<T>;

  //! Construct an oscillator with complex output
  oscillator(T frequency, T sample_rate, T amplitude = 1);

  //! Execute the oscillator
  template<typename U> void operator()(U& output);

private:
  T m_freq;
  T m_rate;
  T m_ampl;
  T m_cos1;
  T m_sin1;
  T m_cos_omega;
  T m_sin_omega;
};

///////////////////////////////////////////////////////////////////////////////

template<typename T>
oscillator<std::complex<T>>::oscillator(T frequency, T sample_rate, T amplitude)
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

template<typename T>
template<typename U>
void oscillator<std::complex<T>>::operator()(U& output)
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

} /* namespace comm */

#endif /* OSCILLATOR_HPP_ */
