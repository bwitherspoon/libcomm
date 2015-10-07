/*
 * Copyright 2015 C. Brett Witherspoon
 */

#include <unistd.h>   // for sysconf
#include <sys/mman.h> // for mmap

#include "comm/math.hpp"
#include "comm/buffer.hpp"

namespace comm
{
namespace buffer
{
namespace
{
size_t system_page_size()
{
    using std::string;
    using std::runtime_error;
#if _POSIX_VERSION >= 200112L
    long size = sysconf(_SC_PAGESIZE);

    if (size == -1)
        throw runtime_error(string(__func__) + "sysconf failed");

    return static_cast<size_t>(size);
#else
    #error "system_page_size: unsupported platform"
#endif
}

void * allocate_mirrored_pages(size_t n)
{
    using std::string;
    using std::runtime_error;

    const int prot = PROT_READ | PROT_WRITE;
    const int flags = MAP_SHARED | MAP_ANONYMOUS;

    // Create an anonymous memory mapping that is initialized to zero
    void * p = mmap(nullptr, 2*n, prot, flags, -1, 0);
    if (p == MAP_FAILED)
        throw runtime_error(string(__func__) + "mmap failed");

    // Remap the pages so the second half mirrors the first
    // FIXME remap_file_pages has been deprecated
    if (remap_file_pages(static_cast<char*>(p) + n, n, 0, 0, 0) == -1)
    {
        munmap(p, 2*n);;
        throw runtime_error(string(__func__) + "remap_file_pages failed");
    }

  return p;
}

void deallocate_mirrored_pages(void * p, std::size_t n)
{
    munmap(static_cast<void*>(p), 2*n);
}
} // namespace anonymous

////////////////////////////////////////////////////////////////////////////////

impl::impl(size_t num_items, size_t item_size)
{
    using comm::math::gcd;

    // Add an extra item to disambiguate full and empty conditions
    num_items += 1;

    // Find the number of items required to end on a page boundary
    const auto page_size = system_page_size();
    const auto items_per_page = page_size / gcd(page_size, item_size);

    if (num_items % items_per_page)
        num_items = (num_items / items_per_page + 1) * items_per_page;

    d_size = num_items * item_size;

    d_base = allocate_mirrored_pages(d_size);
}

impl::~impl()
{
    deallocate_mirrored_pages(d_base, d_size);
}

} // namespace buffer
} // namespace comm
