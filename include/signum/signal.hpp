/*
 * Copyright 2015 C. Brett Witherspoon
 */

#ifndef SIGNUM_SIGNAL_HPP_
#define SIGNUM_SIGNAL_HPP_

#include <cstddef>     // for size_t, ptrdiff_t
#include <type_traits> // for std::is_arithmetic
#include <vector>      // for std::vector

namespace signum
{
template<typename T>
class signal final
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

  static_assert(std::is_arithmetic<T>::value,
                "Template parameter must be arithmetic type");

  signal();

  signal(std::vector<T> & vec);

  ~signal() = default;

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
signal<T>::signal()
  : d_begin(nullptr), d_end(nullptr)
{ }

template<typename T>
signal<T>::signal(std::vector<T> & vec)
  : d_begin(vec.data()), d_end(d_begin + vec.size())
{ }

} // namespace signum
#endif /* SIGNUM_SIGNAL_HPP_ */
