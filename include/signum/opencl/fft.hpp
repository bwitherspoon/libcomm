/*
 * Copyright 2016 C. Brett Witherspoon
 */

#ifndef SIGNUM_OPENCL_FFT_HPP_
#define SIGNUM_OPENCL_FFT_HPP_

#include <complex>

#include <boost/compute/command_queue.hpp>
#include <boost/compute/kernel.hpp>
#include <boost/compute/program.hpp>

namespace signum
{
namespace opencl
{

//! Compute the DFT using the FFT algorithm with OpenCL
class fft
{
public:
    fft(boost::compute::command_queue& queue, std::size_t length);

    fft(const fft&) = delete;

    ~fft() = default;

    fft& operator=(const fft&) = delete;

    std::complex<float>* map(cl_mem_flags flags, const boost::compute::wait_list &events = boost::compute::wait_list());

    boost::compute::event unmap();

    boost::compute::wait_list operator()();

private:
    void *m_host_ptr;
    const std::size_t m_length;
    const std::size_t m_stages;
    boost::compute::command_queue m_queue;
    boost::compute::program m_program;
    std::vector<boost::compute::kernel> m_kernels;
    boost::compute::buffer m_buffer;
    boost::compute::wait_list m_events;
};

} // end namespace opencl
} // end namespace signum

#endif /* SIGNUM_OPENCL_FFT_HPP_ */
