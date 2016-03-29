/*
 * Copyright 2016 C. Brett Witherspoon
 */

#include <iostream>
#include <fstream>
#include <sstream>

#include "signum/opencl/session.hpp"
#include "signum/opencl/platforms.hpp"
#include "signum/opencl/error.hpp"

namespace signum
{
namespace opencl
{

session::session(const std::string &platform, const std::string &program)
    : _platform(platforms::id(platform))
{
    try
    {
        create_context();
        create_devices();
        create_queues();
        create_program(program);
    }
    catch (general_error &e)
    {
        if (_program)
            clReleaseProgram(_program);
        for (const auto &queue : _queues)
            clReleaseCommandQueue(queue);
        if (_context)
            clReleaseContext(_context);
        throw;
    }
}

session::~session()
{
    clReleaseProgram(_program);

    for (const auto &queue : _queues)
        clReleaseCommandQueue(queue);

    clReleaseContext(_context);
}

void session::create_context()
{
    cl_int error = 0;
    cl_context_properties properties[] = { CL_CONTEXT_PLATFORM, 0, 0 };
    properties[1] = reinterpret_cast<cl_context_properties>(_platform),

    _context = clCreateContextFromType(properties, CL_DEVICE_TYPE_ALL, nullptr, nullptr, &error);
    throw_on_error(error, __func__);
}

void session::create_devices()
{
    cl_int error = 0;

    // Get the device list size
    size_t size;
    error = clGetContextInfo(_context, CL_CONTEXT_DEVICES, 0, nullptr, &size);
    throw_on_error(error, __func__);

    // Get the device list
    _devices.resize(size / sizeof(cl_device_id));
    error = clGetContextInfo(_context, CL_CONTEXT_DEVICES, size, _devices.data(), nullptr);
    throw_on_error(error, __func__);
}

void session::create_queues()
{
    cl_int error = 0;

    for (const auto &device : _devices)
    {
        auto queue = clCreateCommandQueue(_context, device, CL_QUEUE_PROFILING_ENABLE, &error);
        throw_on_error(error);
        _queues.push_back(queue);
    }
}

void session::create_program(const std::string &program)
{
    cl_int error = 0;

    std::ifstream file(program);
    if (!file.is_open())
        throw std::invalid_argument("Could not open file: " + program);

    std::ostringstream stream;
    stream << file.rdbuf();
    auto buffer = stream.str();
    auto source = buffer.c_str();

    _program = clCreateProgramWithSource(_context, 1, &source, nullptr, &error);
    throw_on_error(error, __func__);

    error = clBuildProgram(_program, 0, nullptr, nullptr, nullptr, nullptr);

    for (auto &device : _devices)
    {
        size_t log_size = 0;
        clGetProgramBuildInfo(_program, device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &log_size);

        if (log_size > 1)
        {
            std::string log(log_size, '\0');
            clGetProgramBuildInfo(_program, device, CL_PROGRAM_BUILD_LOG, log_size, &log[0], nullptr);
            std::cerr << "ERROR: Program build log: " << std::endl << std::endl;
            std::cerr << log << std::endl;
        }
    }

    throw_on_error(error, __func__);
}

kernel session::create_kernel(const std::string &name, unsigned int device)
{
    return kernel(_context, _queues.at(device), _program, _devices.at(device), name);
}

} // end namespace signum
} // end namespace opencl
