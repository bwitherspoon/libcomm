/*
 * Copyright 2015 C. Brett Witherspoon
 */

#ifndef COMM_BUFFER_HPP_
#define COMM_BUFFER_HPP_

#include <atomic>  // for std::atomic
#include <cstddef> // for std::size_t
#include <limits>  // for std::numeric_limits
#include <memory>  // for std::shared_ptr

namespace comm
{
namespace buffer
{

//! A class for buffer shared data
class impl
{
public:
    explicit impl(size_t num_items, size_t item_size);
    ~impl();
    impl(const impl &) = delete;
    impl(impl &&) = delete;
    impl & operator=(const impl &) = delete;
    impl & operator=(impl &&) = delete;
private:
    template<typename> friend class writer;
    template<typename> friend class reader;
    template<template<typename> class, typename> friend class base;
    void * d_base;
    size_t d_size;
    std::atomic<size_t> d_read;
    std::atomic<size_t> d_write;
};

//! A base class for a buffer
template<template<typename> class U, typename T>
class base
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

    static_assert(std::numeric_limits<size_type>::is_modulo,
                  "buffer::base: modulo arithmetic not supported");

    //! Returns an iterator to the beginning of the buffer
    iterator begin();

    //! Returns a constant iterator to the beginning of the buffer
    const_iterator begin() const;

    //! Returns a constant iterator to the beginning of the buffer
    const_iterator cbegin() const { return begin(); }

    //! Returns an iterator to the end of the buffer
    iterator end();

    //! Returns a constant iterator to the end of the buffer
    const_iterator end() const;

    //! Returns a constant iterator to the end of the buffer
    const_iterator cend() const { return end(); }

    //! Checks whether the buffer is empty
    bool empty() const { return d_impl->d_write == d_impl->d_read; }

    //! Returns the number of items in the buffer
    size_type size() const;

    //! Consume items from the buffer
    void consume(size_type n);

protected:
    explicit base(size_type n);

    explicit base(std::shared_ptr<impl> ptr);

    base(const base &) = default;

    base(base &&) = default;

    base & operator=(const base &) = delete;

    base & operator=(base && other) = delete;

    std::shared_ptr<impl> d_impl;
};

template<template<typename> class T, typename U>
base<T,U>::base(size_type n)
    : d_impl{std::make_shared<impl>(n, sizeof(U))}
{ }

template<template<typename> class T, typename U>
base<T,U>::base(std::shared_ptr<impl> ptr)
    : d_impl{ptr}
{ }

template<template<typename> class T, typename U>
typename base<T,U>::iterator base<T,U>::begin()
{
    const auto base = static_cast<pointer>(d_impl->d_base);
    return base + static_cast<const T<U> *>(this)->position();
}

template<template<typename> class T, typename U>
typename base<T,U>::const_iterator base<T,U>::begin() const
{
    const auto base = static_cast<pointer>(d_impl->d_base);
    return base + static_cast<const T<U> *>(this)->position();
}

template<template<typename> class T, typename U>
typename base<T,U>::iterator base<T,U>::end()
{
    return begin() + size();
}

template<template<typename> class T, typename U>
typename base<T,U>::const_iterator base<T,U>::end() const
{
    return begin() + size();
}

template<template<typename> class T, typename U>
typename base<T,U>::size_type base<T,U>::size() const
{
    const auto max = std::numeric_limits<size_type>::max();
    const auto underflow = max - d_impl->d_size / sizeof(U) + 1;

    auto sz = static_cast<const T<U> *>(this)->offset();

    if (sz >= underflow)
        sz -= underflow;

    return sz;
}

template<template<typename> class T, typename U>
void base<T,U>::consume(size_type n)
{
    const auto overflow = d_impl->d_size / sizeof(U);

    auto pos = static_cast<const T<U> *>(this)->position() + n;

    if (pos >= overflow)
        pos -= overflow;

    static_cast<T<U>*>(this)->position(pos);
}

////////////////////////////////////////////////////////////////////////////////

//! A class to read from a buffer
template<typename T>
class reader : public base<reader,T>
{
public:
    using value_type      = T;
    using size_type       = typename base<reader,T>::size_type;
    using difference_type = typename base<reader,T>::difference_type;
    using reference       = T &;
    using const_reference = const T &;
    using pointer         = T *;
    using const_pointer   = const T *;
    using iterator        = T *;
    using const_iterator  = const T *;

private:
    using base_type = base<reader,value_type>;

    template<typename> friend class writer;

    template<template<typename> class, typename> friend class base;

    explicit reader(std::shared_ptr<impl> ptr);

    size_type offset() const
    {
        return base_type::d_impl->d_write - base_type::d_impl->d_read;
    }

    size_type position() const
    {
        return base_type::d_impl->d_read;
    }

    void position(size_type pos)
    {
        base_type::d_impl->d_read = pos;
    }
};

template<typename T>
reader<T>::reader(std::shared_ptr<impl> ptr)
    : base<reader,T>(ptr)
{ }

////////////////////////////////////////////////////////////////////////////////

//! A class to write to a buffer
template<typename T>
class writer : public base<writer,T>
{
public:
    using value_type      = T;
    using size_type       = typename base<reader,T>::size_type;
    using difference_type = typename base<reader,T>::difference_type;
    using reference       = T &;
    using const_reference = const T &;
    using pointer         = T *;
    using const_pointer   = const T *;
    using iterator        = T *;
    using const_iterator  = const T *;

    explicit writer(size_type n);

    reader<value_type> make_reader();

private:
    using base_type = base<writer,value_type>;

    template<template<typename> class, typename> friend class base;

    size_type offset() const
    {
        return base_type::d_impl->d_read - base_type::d_impl->d_write - 1;
    }

    size_type position() const
    {
        return base_type::d_impl->d_write;
    }

    void position(size_type pos) const
    {
        base_type::d_impl->d_write = pos;
    }
};

template<typename T>
writer<T>::writer(size_type n)
    : base<writer,T>{std::make_shared<impl>(n, sizeof(T))}
{ }

template<typename T>
reader<T> writer<T>::make_reader()
{
    return std::move(reader<T>{base<writer,T>::d_impl});
}

} // namespace buffer
} // namespace comm

#endif /* COMM_BUFFER_HPP_ */
