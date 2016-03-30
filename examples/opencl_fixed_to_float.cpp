/*
 * Copyright 2016 C. Brett Witherspoon
 */

#include <chrono>
#include <iostream>
#include <utility>
#include <tuple>

#include <signum/fixed.hpp>
#include <signum/util/priority.hpp>
#include <signum/opencl/session.hpp>

static const size_t buff_size = 32*1024*1024;

using namespace signum;

void opencl_scalar_fixed_to_float()
{
    opencl::session session("", "opencl_fixed_to_float.cl");

    auto kernel = session.create_kernel("fixed_to_float");

    kernel.create_buffer(0, buff_size / 2, false);
    kernel.create_buffer(1, buff_size, true);

    void *ptr0;
    size_t size0;

    std::tie(ptr0, size0) = kernel.map_buffer(0, true);

    auto buf0 = static_cast<cl_short *>(ptr0);

    for (auto i = 0U; i < size0 / sizeof(cl_short); ++i)
    {
        buf0[i] = i;
    }

    kernel.unmap_buffer(0, std::make_pair(ptr0, size0));

    auto start = std::chrono::steady_clock::now();
    kernel.enqueue(buff_size / sizeof(cl_float));
    kernel.wait();
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << __func__ << ": " << diff.count() / 1e6 << " us " << std::endl;

    void *ptr1;
    size_t size1;

    std::tie(ptr1, size1) = kernel.map_buffer(1, false);

    auto buf1 = static_cast<cl_float *>(ptr1);

    kernel.unmap_buffer(1, std::make_pair(ptr1, size1));

    // The compiler is sensitive about unused variables
    (void)buf1;
}

void opencl_vector_fixed_to_float()
{
    opencl::session session("", "opencl_fixed_to_float.cl");

    auto kernel = session.create_kernel("fixed4_to_float4");

    kernel.create_buffer(0, buff_size / 2, false);
    kernel.create_buffer(1, buff_size, true);

    void *ptr0;
    size_t size0;

    std::tie(ptr0, size0) = kernel.map_buffer(0, true);

    auto buf0 = static_cast<cl_short *>(ptr0);

    for (auto i = 0U; i < size0 / sizeof(cl_short); ++i)
    {
        buf0[i] = i;
    }

    kernel.unmap_buffer(0, std::make_pair(ptr0, size0));

    auto start = std::chrono::steady_clock::now();
    kernel.enqueue(buff_size / sizeof(cl_float) / 4);
    kernel.wait();
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << __func__ << ": " << diff.count() / 1e6 << " us " << std::endl;

    void *ptr1;
    size_t size1;

    std::tie(ptr1, size1) = kernel.map_buffer(1, false);

    auto buf1 = static_cast<cl_float *>(ptr1);

    kernel.unmap_buffer(1, std::make_pair(ptr1, size1));

    // The compiler is sensitive about unused variables
    (void)buf1;
}

void cpu_fixed_to_float()
{
    std::vector<short> buf0(buff_size / sizeof(short));
    std::vector<float> buf1(buff_size / sizeof(short));

    for (auto i = 0U; i < buf0.size(); ++i)
    {
        buf0[i] = i;
    }

    auto start = std::chrono::steady_clock::now();
    for (auto i = 0U; i < buf0.size(); ++i)
    {
        buf1[i] = signum::fixed_to_float<float>(buf0[i]);
    }
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << __func__ << ": " << diff.count() / 1e6 << " us " << std::endl;

    // The compiler is sensitive about unused variables
    (void)buf1;
}

int main (int argc, char *argv[])
{
    util::set_realtime_priority();

    cpu_fixed_to_float();
    opencl_scalar_fixed_to_float();
    opencl_vector_fixed_to_float();

    return 0;
}
