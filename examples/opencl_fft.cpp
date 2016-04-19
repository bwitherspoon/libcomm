/*
 * Copyright 2016 C. Brett Witherspoon
 */

#include <algorithm>
#include <chrono>
#include <cmath>
#include <complex>
#include <iostream>
#include <stdexcept>

#include <boost/preprocessor/stringize.hpp>
#include <boost/program_options.hpp>
#include <boost/compute/core.hpp>

namespace compute = boost::compute;
namespace po = boost::program_options;

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

    __kernel void twiddle(__global float2 *x, uint n)
    {
        uint k = get_global_id(0);

        // e^{-j 2 \pi k/n)
        x[k] = cos((float2)(2*M_PI_F*k/n, 2*M_PI_F*k/n + M_PI_F/2));
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

        // Bufferfly
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
        float2 z = (float2)(v[i+j].x*w.x - v[i+j].y*w.y, v[i+j].x*w.y + v[i+j].y*w.x);

        // Butterfly
        v[i+j] = v[i] - z;
        v[i]   = v[i] + z;
    }
);

bool ispowertwo(unsigned int x)
{
    return !(x == 0 || (x & (x - 1)));
}

} // end anonymous namespace

int main(int argc, char *argv[])
{
    size_t length;

    po::options_description desc("Supported options");
    desc.add_options()
        ("help,h", "print help message")
        ("length,l", po::value<size_t>(&length)->default_value(8), "set FFT length");
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cerr << desc << std::endl;
        return 1;
    }

    if (!ispowertwo(length))
    {
        std::cerr << "FFT Length must be a power of two" << std::endl;
        return 1;
    }

    const size_t stages = static_cast<size_t>(log2(length));

    compute::device device = compute::system::default_device();
    compute::context context(device);
    compute::command_queue queue(context, device, compute::command_queue::enable_profiling);

    // Print some device information
    std::cout << device.platform().name() << ": " << device.name() << std::endl;
    std::cout << "Global memory size: " << device.global_memory_size() << std::endl;
    std::cout << "Local memory size: " << device.local_memory_size() << std::endl;
    std::cout << "Compute units: " << device.compute_units() << std::endl;
    std::cout << "Preferred vector width: " <<device.preferred_vector_width<float>() << std::endl;

    // Build program and create kernels
    auto program = compute::program::create_with_source(source, context);

    compute::kernel window_kernel;
    compute::kernel reorder_kernel;
    compute::kernel stage1_kernel;
    compute::kernel stagex_kernel;

    try
    {
        program.build();
        window_kernel = program.create_kernel("window");
        reorder_kernel = program.create_kernel("reorder");
        stage1_kernel = program.create_kernel("stage1");
        stagex_kernel = program.create_kernel("stagex");
    }
    catch (compute::opencl_error &error)
    {
        std::cerr << error.what() << ": " << std::endl << program.build_log();
        return 1;
    }

    // Allocate kernel buffers
    auto flags = compute::buffer::read_write | compute::buffer::alloc_host_ptr;
    auto size = length * sizeof(std::complex<float>);
    compute::buffer buffer(context, size, flags);

    // Set kernel arguments
    window_kernel.set_arg(0, buffer);
    window_kernel.set_arg(1, length);

    reorder_kernel.set_arg(0, buffer);
    reorder_kernel.set_arg(1, stages);

    stage1_kernel.set_arg(0, buffer);

    stagex_kernel.set_arg(0, buffer);
    stagex_kernel.set_arg(1, length);
    stagex_kernel.set_arg(2, stages);
    stagex_kernel.set_arg(3, 1);

    // Initialize input buffer
    auto ptr = queue.enqueue_map_buffer(buffer,
                                        compute::command_queue::map_write, 0,
                                        buffer.size());
    auto buf = static_cast<std::complex<float>*>(ptr);

    std::default_random_engine eng;
    std::normal_distribution<> dist{0, 1};
    auto rand = std::bind(dist, eng);
    std::generate(buf, buf + length, rand);

    std::cout << "Input: " << std::endl;
    for (size_t i = 0; i < length; ++i) std::cout << buf[i] << std::endl;

    queue.enqueue_unmap_buffer(buffer, ptr).wait();

    compute::wait_list events;

    // Enqueue kernels
    //auto window_event = queue.enqueue_1d_range_kernel(window_kernel, 0, length/2, 0);
    events.insert(queue.enqueue_1d_range_kernel(reorder_kernel, 0, length, 0));
    events.insert(queue.enqueue_1d_range_kernel(stage1_kernel, 0, length/2, 0, events[0]));

    for (size_t i = 2; i <= stages; ++i)
    {
        stagex_kernel.set_arg(3, i);

        auto groups = length >> i;
        auto items = 1 << i;

        for (size_t g = 0; g < groups; ++g)
        {
            events.insert(queue.enqueue_1d_range_kernel(stagex_kernel, g * items, items / 2, 0, events[i-1]));
        }
    }

    events.wait();

    // Print profiling information
    std::chrono::nanoseconds time{0};
    for (const auto &event : events)
    {
        time += event.duration<std::chrono::nanoseconds>();
    }
    std::cout << "Execute time: " << time.count() << " ns" << std::endl;

    // Print output buffer
    ptr = queue.enqueue_map_buffer(buffer,
                                   compute::command_queue::map_read, 0,
                                   buffer.size());
    buf = static_cast<std::complex<float>*>(ptr);

    std::cout << "Output: " << std::endl;
    for (size_t i = 0; i < length; ++i) std::cout << buf[i] << std::endl;

    queue.enqueue_unmap_buffer(buffer, ptr).wait();

    return 0;
}
