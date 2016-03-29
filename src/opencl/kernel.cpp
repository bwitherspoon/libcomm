/*
 * Copyright 2016 C. Brett Witherspoon
 */

#include <utility>

#include "signum/opencl/error.hpp"
#include "signum/opencl/kernel.hpp"

namespace signum
{
namespace opencl
{

kernel::kernel(cl_context context, cl_command_queue queue, cl_program program,
               cl_device_id device, const std::string &name)
    : _context(context),
      _queue(queue),
      _device(device),
      _event(nullptr)
{
    cl_int error = 0;
    cl_uint num_args;

    _kernel = clCreateKernel(program, name.c_str(), &error);
    throw_on_error(error, __func__);

    error = clGetKernelInfo(_kernel, CL_KERNEL_NUM_ARGS, sizeof(num_args), &num_args, nullptr);
    throw_on_error(error, __func__); // FIXME leaks kernel

    _memory.resize(num_args);

    error = clRetainCommandQueue(_queue);
    throw_on_error(error, __func__); // FIXME leaks kernel
}

kernel::~kernel()
{
    clReleaseCommandQueue(_queue);

    for (auto &mem : _memory)
        if (mem) clReleaseMemObject(mem);

    clReleaseKernel(_kernel);
}

void kernel::create_buffer(unsigned int index, unsigned int size, bool write)
{
    cl_int error;
    cl_mem mem = _memory.at(index);
    cl_mem_flags flags = ((write) ? CL_MEM_WRITE_ONLY : CL_MEM_READ_ONLY) | CL_MEM_ALLOC_HOST_PTR;

    mem = clCreateBuffer(_context, flags, size, nullptr, &error);
    throw_on_error(error, __func__);

    if (mem) clReleaseMemObject(_memory[index]);

    _memory[index] = mem;

    error = clSetKernelArg(_kernel, index, sizeof(_memory[index]), &_memory[index]);
    throw_on_error(error, __func__);
}

std::pair<void *,size_t> kernel::map_buffer(unsigned int index, bool write)
{
    cl_int error;
    cl_mem mem = _memory.at(index);
    cl_mem_flags flags = (write) ? CL_MAP_WRITE : CL_MAP_READ;
    size_t size;

    error = clGetMemObjectInfo(mem, CL_MEM_SIZE, sizeof(size_t), &size, nullptr);
    throw_on_error(error, __func__);

    void *buff = clEnqueueMapBuffer(_queue, mem, CL_TRUE, flags, 0, size, 0, nullptr, nullptr, &error);
    throw_on_error(error, __func__);

    return std::make_pair(buff, size);
}

void kernel::unmap_buffer(unsigned int index, std::pair<void *, size_t> buff)
{
    cl_int error;
    cl_mem mem = _memory.at(index);

    error = clEnqueueUnmapMemObject(_queue, mem, buff.first, 0, nullptr, nullptr);
    throw_on_error(error, __func__);
}

void kernel::enqueue(size_t worksize)
{
    cl_int error;
    error = clEnqueueNDRangeKernel(_queue, _kernel, 1, nullptr, &worksize, nullptr, 0, nullptr, &_event);
    throw_on_error(error, __func__);
}

void kernel::wait()
{
    cl_int error;
    error = clWaitForEvents(1, &_event);
    throw_on_error(error, __func__);
}

} // end namespace opencl
} // end namespace signum
