/*
 * Copyright 2015 C. Brett Witherspoon
 */

#ifndef COMM_MUTATOR_HPP_
#define COMM_MUTATOR_HPP_

#include <cstdddef> // for std::size_t
#include <vector>

#include "comm/block.hpp"

namespace comm
{
namespace buffer
{
template<typename> class reader;
template<typename> class writer;
}

template<typename> class signal;

/**
 * A signal processing block with inputs and outputs
 */
template<typename T, typename U>
class mutator : public block
{
public:
  ~mutator() = default;

  void operator()() final;

  bool ready() final;

  //! Connect the output of this block to the input of another
  block & operator>>(block & other) final;

  //! Execute a mutator given an input and output
  virtual int operator()(const std::vector<comm::signal<T>> & input,
                         std::vector<comm::signal<U>> & output) = 0;

protected:
  //! Default construct a mutator
  mutator();

  /**
   *  Construct a block
   *  @param ratio the ratio of input items to output items
   *  @param threshold the minimum number if input items required
   */
  mutator(float ratio, std::size_t threshold);

private:
  const float d_ratio;
  const std::size_t d_thresh;
  std::vector<comm::buffer::reader<T>> d_readers;
  std::vector<comm::buffer::writer<U>> d_writers;
  std::vector<comm::signal<T>> d_input;
  std::vector<comm::signal<U>> d_output;
};

template<typename T, typename U>
mutator<T,U>::mutator()
  : d_ratio(1), d_thresh(0)
{ }

template<typename T, typename U>
mutator<T,U>::mutator(float ratio, std::size_t thresh)
  : d_ratio(ratio), d_thresh(thresh)
{ }

template<typename T, typename U>
void mutator<T,U>::operator()()
{
  for (auto buf = d_readers.begin(), sig = d_input.begin();
       buf < d_readers.end() && sig < d_input.end(); ++buf, ++sig)
    *sig = *buf;

  auto consumed = operator()(d_input, d_output);
  auto produced = d_ratio * consumed;

  for (auto & reader : d_readers)
    reader.consume(consumed);

  for (auto & writer : d_writers)
    writer.consume(produced);
}

template<typename T, typename U>
bool mutator<T,U>::ready()
{
  if (d_readers.empty() || d_writers.empty())
    return false;

  const bool enough_input = d_input[0].size() >= d_thresh;
  const bool enough_output = d_output[0].size() >= d_ratio * d_input[0].size();

  return enough_input && enough_output;
}

template<typename T, typename U>
block & mutator<T,U>::operator>>(block & other)
{
  // TODO
  return *this;
}

} /* namespace comm */

#endif /* COMM_MUTATOR_HPP_ */

