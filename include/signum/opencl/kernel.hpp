/*
 * Copyright 2016 C. Brett Witherspoon
 */

#ifndef SIGNUM_OPENCL_KERNEL_HPP_
#define SIGNUM_OPENCL_KERNEL_HPP_

#include <vector>

#include <CL/cl.h>

namespace signum
{
namespace opencl
{

class session;

class kernel final
{
public:
    ~kernel();

    void create_buffer(unsigned int index, unsigned int size, bool write);

    std::pair<void *, size_t> map_buffer(unsigned int index, bool write);

    void unmap_buffer(unsigned int index, std::pair<void *, size_t> buff);

    void enqueue(size_t worksize);

    void wait();

private:
    friend session;

    kernel(cl_context context, cl_command_queue queue, cl_program program,
           cl_device_id device, const std::string &name);

    cl_context _context;
    cl_command_queue _queue;
    cl_device_id _device;
    cl_event _event;
    cl_kernel _kernel;
    std::vector<cl_mem> _memory;
};

} // end namespace opencl
} // end namespace signum

#endif /* SIGNUM_OPENCL_KERNEL_HPP_ */
