/*
 * Copyright 2015 C. Brett Witherspoon
 */

#ifndef INCLUDE_COMM_BUFFER_HPP_
#define INCLUDE_COMM_BUFFER_HPP_

#include <sys/mman.h>
#include <unistd.h>
#include <atomic>
#include <cerrno>
#include <cstddef>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>

#include "comm/math.hpp"

namespace comm
{
namespace detail
{

template<typename ValueType>
class buffer_shared;

} /* namespace detail */

struct input_buffer_tag;

struct output_buffer_tag;

template<typename ValueType, typename Tag>
class buffer;

//! An input buffer
template<typename ValueType>
using input_buffer = buffer<ValueType, input_buffer_tag>;

//! An output buffer
template<typename ValueType>
using output_buffer = buffer<ValueType, output_buffer_tag>;

//! A buffer input and output pair
template<typename ValueType>
using buffer_pair = std::pair<
    std::unique_ptr<input_buffer<ValueType>>,
        std::unique_ptr<output_buffer<ValueType>>>;

template<typename ValueType>
buffer_pair<ValueType> make_buffer_pair(std::size_t);

} /* namespace comm */

namespace comm
{
//! An exception thrown on buffer construction errors
struct buffer_error : public std::system_error
{
  explicit buffer_error(const std::string& where, const std::string& what) :
      std::system_error(errno, std::generic_category(), where + ": " + what)
  {
  }
};

////////////////////////////////////////////////////////////////////////////////

namespace detail
{
/*!
 * Shared buffer data
 */
template<typename ValueType>
class buffer_shared
{
public:
  friend input_buffer<ValueType>;
  friend output_buffer<ValueType>;

  explicit buffer_shared(std::size_t n);

  buffer_shared(const buffer_shared<ValueType>&) = delete;

  buffer_shared(buffer_shared<ValueType>&&) = delete;

  buffer_shared<ValueType>&
  operator=(const buffer_shared<ValueType>&) = delete;

  buffer_shared<ValueType>&
  operator=(const buffer_shared<ValueType>&&) = delete;

  ~buffer_shared();

  static constexpr std::size_t max_size();

private:
  static std::size_t sizeof_pages();

  static ValueType* allocate_pages(std::size_t n);

  static void deallocate_pages(ValueType* addr, std::size_t size);

  ValueType* data_;
  std::size_t size_;
  std::atomic<std::size_t> in_;
  std::atomic<std::size_t> out_;
};

/*! \brief The maximum number of items the buffer can hold
 *
 *   This value is limited to prevent overflow during mmap calls and the index
 *   wrapping logic.
 */
template<typename ValueType>
constexpr std::size_t buffer_shared<ValueType>::max_size()
{
  return std::numeric_limits<std::size_t>::max() /
      comm::math::max(sizeof(ValueType), static_cast<std::size_t>(2));
}

//! Determine the system page size
template<typename ValueType>
std::size_t buffer_shared<ValueType>::sizeof_pages()
{
  // TODO support windows
#if _POSIX_VERSION >= 200112L
  long pagesize = sysconf(_SC_PAGESIZE);

  if (pagesize == -1)
    throw buffer_error(__func__, "sysconf failed");

  return static_cast<std::size_t>(pagesize);
#else
  #error("buffer: unsupported platform")
#endif
}

//! Allocates enough pages for at least n items
template<typename ValueType>
ValueType* buffer_shared<ValueType>::allocate_pages(std::size_t n)
{
  // TODO support windows
  const auto size = n * sizeof(ValueType);
  const int prot = PROT_READ | PROT_WRITE;
  const int flags = MAP_SHARED | MAP_ANONYMOUS;

  // Create an anonymous memory mapping that is initialized to zero
  void* addr = mmap(nullptr, 2*size, prot, flags, -1, 0);
  if (addr == MAP_FAILED)
    throw buffer_error(__func__, "mmap failed");

  // Remap the pages so the second half mirrors the first
  // FIXME remap_file_pages has been deprecated
  if (remap_file_pages(static_cast<char*>(addr) + size, size, 0, 0, 0) == -1)
  {
    buffer_error e(__func__, "remap_file_pages failed");
    munmap(addr, 2*size);;
    throw e;
  }

  return static_cast<ValueType*>(addr);
}

//! Deallocates pages previously allocated with allocate_pages
template<typename ValueType>
void
buffer_shared<ValueType>::deallocate_pages(ValueType* addr, std::size_t size)
{
  munmap(static_cast<void*>(addr), 2*size*sizeof(ValueType));
}

//! Constructs a buffer with space for at least n items
template<typename ValueType>
buffer_shared<ValueType>::buffer_shared(std::size_t n)
{
  using comm::math::gcd;

  // Add an extra item to disambiguate full and empty conditions
  n += 1;

  // Determine the number of items >= n required to end on a page boundary
  const auto pagesize = sizeof_pages();
  const auto granularity = pagesize / gcd(pagesize, sizeof(ValueType));

  if (n % granularity)
    size_ = (n / granularity + 1) * granularity;
  else
    size_ = n;

  // Limit size to prevent overflow
  if (size_ > max_size())
    throw std::invalid_argument(__func__ + std::string(": size limit exceeded"));

  data_ = allocate_pages(size_);
}

//! Deallocates resources
template<typename ValueType>
buffer_shared<ValueType>::~buffer_shared()
{
  deallocate_pages(data_, size_);
}

} /* namespace detail */

////////////////////////////////////////////////////////////////////////////////

//! \cond
//! An input buffer tag
struct input_buffer_tag
{
};
//! An output buffer tag
struct output_buffer_tag
{
};
//! \endcond

//! Make an input and output buffer pair */
template<typename T>
buffer_pair<T> make_buffer_pair(std::size_t n)
{
  auto impl = std::make_shared<detail::buffer_shared<T>>(n);

  // TODO C++14 introduces make_unique
  auto in = std::unique_ptr<input_buffer<T>>(new input_buffer<T>(impl));

  auto out = std::unique_ptr<output_buffer<T>>(new output_buffer<T>(impl));

  return std::make_pair(std::move(in), std::move(out));
}

////////////////////////////////////////////////////////////////////////////////

/**
 * \brief A single producer single consumer thread-safe buffer
 */
template<typename ValueType, typename Tag>
class buffer
{
public:
  using value_type = ValueType;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = value_type&;
  using const_reference = const reference;
  using pointer = value_type*;
  using const_pointer = const pointer;
  using iterator = pointer;
  using const_iterator = const_pointer;

  static_assert(std::numeric_limits<size_type>::is_modulo,
      "buffer: size_type does not support modulo arithmetic");

  static const size_type alignment = alignof(value_type);

  friend buffer_pair<value_type> make_buffer_pair<value_type>(size_t);

  buffer(const buffer<ValueType, Tag>&) = delete;

  buffer(buffer<ValueType, Tag>&&) = delete;

  buffer<ValueType, Tag>&
  operator=(const buffer<ValueType, Tag>&) = delete;

  buffer<ValueType, Tag>&
  operator=(const buffer<ValueType, Tag>&&) = delete;

  pointer data()
  {
    return shared_->data_ + index();
  }

  const_pointer data() const
  {
    return shared_->data_ + index();
  }

  //! Returns an iterator to the beginning of the buffer
  iterator begin()
  {
    return shared_->data_ + index();
  }

  //! Returns an iterator to the beginning of the buffer
  const_iterator begin() const
  {
    return shared_->data_ + index();
  }

  //! Returns an iterator to the beginning of the buffer
  const_iterator cbegin() const
  {
    return shared_->data_ + index();
  }

  //! Returns an iterator to the end of the buffer
  iterator end()
  {
    return begin() + size();
  }

  //! Returns a iterator to the end of the buffer
  const_iterator end() const
  {
    return begin() + size();
  }

  //! Returns a iterator to the end of the buffer
  const_iterator cend() const
  {
    return begin() + size();
  }

  //! Checks whether the buffer is empty
  bool empty() const
  {
    return shared_->in_ == shared_->out_;
  };

  //! Returns the number of items in the buffer
  size_type size() const;

  //! Returns the number of items that can be held plus one
  size_type capacity() const
  {
    return shared_->size_;
  }

  //! The maximum number of items that can be held in a buffer
  static constexpr size_type max_size()
  {
    return detail::buffer_shared<ValueType>::max_size();
  }

  //! Advances the current position in the buffer
  void advance(size_type n);

private:
  explicit buffer(std::shared_ptr<detail::buffer_shared<value_type>>& shared) :
      shared_(shared)
  {
  }

  size_type index() const
  {
    return index_dispatch(Tag());
  }

  size_type index_dispatch(input_buffer_tag) const
  {
    return shared_->in_;
  }

  size_type index_dispatch(output_buffer_tag) const
  {
    return shared_->out_;
  }

  void index(size_type i)
  {
    index_dispatch(i, Tag());
  }

  void index_dispatch(size_type i, input_buffer_tag)
  {
    shared_->in_ = i;
  }

  void index_dispatch(size_type i, output_buffer_tag)
  {
    shared_->out_ = i;
  }

  size_type distance() const
  {
    return distance_dispatch(Tag());
  }

  size_type distance_dispatch(input_buffer_tag) const
  {
    return shared_->out_ - shared_->in_ - 1;
  }

  size_type distance_dispatch(output_buffer_tag) const
  {
    return shared_->in_ - shared_->out_;
  }

  size_type underflow() const
  {
    return std::numeric_limits<size_type>::max() - capacity() + 1;
  }

  std::shared_ptr<detail::buffer_shared<value_type>> shared_;
};

template<typename ValueType, typename Tag>
typename buffer<ValueType, Tag>::size_type
buffer<ValueType, Tag>::size() const
{
  size_type n = distance();

  if (n >= underflow())
    n -= underflow();

  return n;
}

template<typename ValueType, typename Tag>
void buffer<ValueType, Tag>::advance(size_type n)
{
  size_type i = index() + n;

  if (i >= capacity())
    i -= capacity();

  index(i);
}

} /* namespace comm */
#endif /* INCLUDE_COMM_BUFFER_HPP_ */
