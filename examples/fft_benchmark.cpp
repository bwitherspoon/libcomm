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

#include <signum/opencl/fft.hpp>

namespace compute = boost::compute;
namespace po = boost::program_options;
namespace opencl = signum::opencl;

int main(int argc, char *argv[])
{
    size_t length;

    po::options_description desc("Supported options");
    desc.add_options()
        ("help,h", "print help message")
        ("length,l", po::value<size_t>(&length)->default_value(8), "set FFT length")
        ("verbose,v", "print verbose messages");
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cerr << desc << std::endl;
        return 1;
    }

    compute::device device = compute::system::default_device();
    compute::context context(device);
    compute::command_queue queue(context, device, compute::command_queue::enable_profiling);

    // Print some device information
    std::cout << device.platform().name() << ": " << device.name() << std::endl;
    std::cout << "Global memory size: " << device.global_memory_size() << std::endl;
    std::cout << "Local memory size: " << device.local_memory_size() << std::endl;
    std::cout << "Compute units: " << device.compute_units() << std::endl;
    std::cout << "Preferred vector width: " <<device.preferred_vector_width<float>() << std::endl;

    // Create FFT object
    opencl::fft fft(queue, length);

    // Initialize input buffer
    auto input = fft.map(compute::command_queue::map_write);
    std::default_random_engine eng;
    std::normal_distribution<> dist{0, 1};
    auto rand = std::bind(dist, eng);
    std::generate(input, input + length, rand);
    if (vm.count("verbose"))
    {
        std::cout << "Input: " << std::endl;
        for (size_t i = 0; i < length; ++i) std::cout << input[i] << std::endl;
    }
    fft.unmap().wait();

    // Enqueue kernels
    auto events = fft();

    events.wait();

    // Print profiling information
    std::chrono::nanoseconds time{0};
    for (const auto &event : events)
    {
        time += event.duration<std::chrono::nanoseconds>();
    }
    std::cout << "Execute time: " << time.count() << " ns" << std::endl;

    // Print output buffer
    auto output = fft.map(compute::command_queue::map_read);
    if (vm.count("verbose"))
    {
        std::cout << "Output: " << std::endl;
        for (size_t i = 0; i < length; ++i) std::cout << output[i] << std::endl;
    }
    fft.unmap().wait();

    return 0;
}
