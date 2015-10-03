/*
 * Copyright 2015 C. Brett Witherspoon
 */

#ifndef ALIGNED_ALLOCATOR_HPP_
#define ALIGNED_ALLOCATOR_HPP_

#include <cstdlib>
#include <cstddef>
#include <new>

namespace comm
{
/**
 * An STL compatible 16-byte aligned memory allocator
 */
template <typename T>
class aligned_allocator
{
public:
  using size_type       = std::size_t;
  using difference_type = std::ptrdiff_t;
  using pointer         = T*;
  using const_pointer   = const T*;
  using value_type      = T;

  aligned_allocator() { };

  template <typename U>
  aligned_allocator(const aligned_allocator<U>& other) { };

  ~aligned_allocator() { };

  pointer allocate(size_type n)
  {
    void *p = aligned_alloc(16, n * sizeof(value_type));
    if (p == nullptr)
      throw std::bad_alloc();
    else
      return static_cast<pointer>(p);
  }

  void deallocate(pointer p, size_type)
  {
    std::free(p);
  }
};

template <typename T, typename U>
inline bool operator==(const aligned_allocator<T>&, const aligned_allocator<U>&)
{
  return true;
}

template <typename T, typename U>
inline bool operator!=(const aligned_allocator<T>&, const aligned_allocator<U>&)
{
  return false;
}

} /* namespace comm */

#endif /* ALIGNED_ALLOCATOR_HPP_ */

