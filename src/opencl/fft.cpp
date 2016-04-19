/*
 * Copyright 2016 C. Brett Witherspoon
 */

#include <iostream>
#include <stdexcept>

#include <boost/preprocessor/stringize.hpp>

#include "signum/opencl/fft.hpp"

namespace
{
const char *source = BOOST_PP_STRINGIZE(

    __kernel void window(__global float4 *x, uint n)
    {
        uint i = get_global_id(0);

        // Hamming
        float4 w;
        w.xy = 0.54 - 0.46 * cos(2*M_PI_F*(2*i)/(n-1));
        w.zw = 0.54 - 0.46 * cos(2*M_PI_F*(2*i+1)/(n-1));

        x[i] = w * x[i];
    }

    __kernel void twiddle(__global float2 *w, uint n)
    {
        uint k = get_global_id(0);

        // e^{-j 2 \pi k/n)
        w[k] = cos((float2)(2*M_PI_F*k/n, 2*M_PI_F*k/n + M_PI_F/2));
    }

    __kernel void reorder(__global float2 *x, uint s)
    {
        uint i = get_global_id(0);

        // Reverse bits
        uint j = i;
        j = (j & 0xFFFF0000) >> 16 | (j & 0x0000FFFF) << 16;
        j = (j & 0xFF00FF00) >> 8  | (j & 0x00FF00FF) << 8;
        j = (j & 0xF0F0F0F0) >> 4  | (j & 0x0F0F0F0F) << 4;
        j = (j & 0xCCCCCCCC) >> 2  | (j & 0x33333333) << 2;
        j = (j & 0xAAAAAAAA) >> 1  | (j & 0x55555555) << 1;

        // Adjust number of bits for log2(length)
        j >>= 32 - s;

        // Swap items
        if (i < j)
        {
            float2 tmp = x[i];
            x[i] = x[j];
            x[j] = tmp;
        }
    }

    __kernel void stage1(__global float4 *v)
    {
        uint i = get_global_id(0);

        // Butterfly
        float2 tmp = v[i].xy;
        v[i].xy = v[i].xy + v[i].zw;
        v[i].zw = tmp - v[i].zw;
    }

    __kernel void stagex(__global float2 *v, uint n, uint m, uint s)
    {
        // Indices
        uint i = get_global_id(0);
        uint j = 1 << (s - 1);

        // Twiddle
        uint mask = ~(0xFFFFFFFF << (s - 1));
        uint k = (i & mask) * (1 << (m - s));
        float2 w = cos((float2)(2*M_PI_F*k/n, 2*M_PI_F*k/n + M_PI_F/2));

        // Complex multiply
        float2 z;
        z.x = v[i+j].x*w.x - v[i+j].y*w.y;
        z.y = v[i+j].x*w.y + v[i+j].y*w.x;

        // Butterfly
        v[i+j] = v[i] - z;
        v[i] = v[i] + z;
    }
);

inline bool ispow2(std::size_t x)
{
    return !(x == 0 || (x & (x - 1)));
}

} // end anonymous namespace

namespace signum
{
namespace opencl
{

fft::fft(boost::compute::command_queue& queue, std::size_t length)
    : m_host_ptr(nullptr),
      m_length(length),
      m_stages(static_cast<size_t>(log2(length))),
      m_queue(queue)
{
    namespace compute = boost::compute;

    if (!ispow2(length))
        throw std::invalid_argument("Length must be a power of two");

    // Build program
    m_program = compute::program::create_with_source(source, m_queue.get_context());

    // Create kernels
    try
    {
        m_program.build();
        m_kernels.push_back(m_program.create_kernel("reorder"));
        m_kernels.push_back(m_program.create_kernel("stage1"));
        m_kernels.push_back(m_program.create_kernel("stagex"));
    }
    catch (compute::opencl_error &error)
    {
        std::cerr << error.what() << ": " << std::endl << m_program.build_log();
        throw;
    }

    // Allocate buffers
    const auto flags = compute::buffer::read_write | compute::buffer::alloc_host_ptr;
    const auto size = length * sizeof(std::complex<float>);
    m_buffer = compute::buffer(queue.get_context(), size, flags);

    // Set kernel arguments
    m_kernels[0].set_arg(0, m_buffer);
    m_kernels[0].set_arg(1, m_stages);

    m_kernels[1].set_arg(0, m_buffer);

    m_kernels[2].set_arg(0, m_buffer);
    m_kernels[2].set_arg(1, m_length);
    m_kernels[2].set_arg(2, m_stages);
    m_kernels[2].set_arg(3, 1);
}

std::complex<float>* fft::map(cl_mem_flags flags, const boost::compute::wait_list &events)
{
    namespace compute = boost::compute;

    if (m_host_ptr != nullptr)
        return static_cast<std::complex<float>*>(m_host_ptr);

    m_host_ptr = m_queue.enqueue_map_buffer(m_buffer, flags, 0, m_buffer.size(), events);

    return static_cast<std::complex<float>*>(m_host_ptr);
}

boost::compute::event fft::unmap()
{
    if (m_host_ptr != nullptr)
    {
        auto event = m_queue.enqueue_unmap_buffer(m_buffer, m_host_ptr);
        m_host_ptr = nullptr;
        return std::move(event);
    }
    else
    {
        return boost::compute::event();
    }
}

boost::compute::wait_list fft::operator()()
{
    if (m_host_ptr != nullptr)
        throw std::runtime_error("OpenCL buffer still mapped to host");

    m_events.clear();

    m_events.insert(m_queue.enqueue_1d_range_kernel(m_kernels[0], 0, m_length, 0));
    m_events.insert(m_queue.enqueue_1d_range_kernel(m_kernels[1], 0, m_length/2, 0, m_events[0]));

    for (size_t i = 2; i <= m_stages; ++i)
    {
        m_kernels[2].set_arg(3, i);

        auto groups = m_length >> i;
        auto items = 1 << i;

        for (size_t g = 0; g < groups; ++g)
        {
            m_events.insert(m_queue.enqueue_1d_range_kernel(m_kernels[2], g*items, items/2, 0, m_events[i-1]));
        }
    }

    return m_events;
}

} // end namespace opencl
} // end namespace signum
