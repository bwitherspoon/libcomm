/*
 * Copyright 2015, 2016 C. Brett Witherspoon
 */

#ifndef SIGNUM_BUFFER_HPP_
#define SIGNUM_BUFFER_HPP_

#include <array>
#include <cstddef> // for size_t, ptrdiff_t
#include <vector>

namespace signum
{
/**
 * A buffer object represents a contiguous region of memory. A buffer object
 * does not have ownership of the memory it refers too. It is the responsibility
 * of the application to ensure the memory region remains valid for the lifetime
 * of the buffer object.
 */

template<typename T>
class buffer final
{
public:
  using value_type      = T;
  using size_type       = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference       = T &;
  using const_reference = const T &;
  using pointer         = T *;
  using const_pointer   = const T *;
  using iterator        = T *;
  using const_iterator  = const T *;

  buffer();

  buffer(std::vector<T> &vec);

  template<std::size_t N>
  buffer(std::array<T, N> &arr);

  ~buffer() = default;

  iterator begin() { return d_begin; }

  const_iterator begin() const { return d_begin; }

  const_iterator cbegin() const { return begin(); }

  iterator end() { return d_end; }

  const_iterator end() const { return d_end; }

  const_iterator cend() const { return begin(); }

  bool empty() const { return d_end == d_begin; }

  size_type size() const { return d_end - d_begin; };

  reference operator[](size_type pos) { return d_begin[pos]; }

  const_reference operator[](size_type pos) const { return d_begin[pos]; }

private:
  pointer d_begin;
  pointer d_end;
};

template<typename T>
buffer<T>::buffer()
    : d_begin(nullptr), d_end(nullptr)
{ }

template<typename T>
buffer<T>::buffer(std::vector<T> &vec)
    : d_begin(vec.data()), d_end(vec.data() + vec.size())
{ }

template<typename T>
template<std::size_t N>
buffer<T>::buffer(std::array<T, N> &arr)
    : d_begin(arr.begin()), d_end(arr.end())
{ }

} // namespace signum
#endif /* SIGNUM_BUFFER_HPP_ */
