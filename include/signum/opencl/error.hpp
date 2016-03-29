/*
 * Copyright 2016 C. Brett Witherspoon
 */

#ifndef SIGNUM_OPENCL_ERROR_HPP_
#define SIGNUM_OPENCL_ERROR_HPP_

#include <stdexcept>
#include <string>

#include <CL/cl.h>
#include <CL/cl_ext.h>

namespace signum
{
namespace opencl
{

struct general_error : public std::runtime_error
{
    general_error(const std::string &what, const std::string &where)
        : runtime_error(where + ": OpenCL: " + what)
    { }
};

struct platform_not_found : public general_error
{
    platform_not_found(const std::string &where)
        : general_error("Platform not found", where)
    { }
};

struct device_not_found : public general_error
{
    device_not_found(const std::string &where)
        : general_error("Device not found", where)
    { }
};

struct invalid_value : public general_error
{
    invalid_value(const std::string &where)
        : general_error("Invalid value", where)
    { }
};

struct out_of_host_memory : public general_error
{
    out_of_host_memory(const std::string &where)
        : general_error("Out of host memory", where)
    { }
};

struct invalid_platform : public general_error
{
    invalid_platform(const std::string &where)
        : general_error("Invalid platform", where)
    { }
};

struct build_program_failure : public general_error
{
    build_program_failure(const std::string &where)
        : general_error("Build program failure", where)
    { }
};

struct invalid_kernel_args : public general_error
{
    invalid_kernel_args(const std::string &where)
        : general_error("Invalid kernel arguments", where)
    { }
};

static inline void throw_on_error(const cl_int error, const std::string &where = "(unknown)")
{
    switch (error)
    {
    case CL_SUCCESS:
        return;
    case CL_PLATFORM_NOT_FOUND_KHR:
        throw platform_not_found(where);
    case CL_INVALID_VALUE:
        throw invalid_value(where);
    case CL_OUT_OF_HOST_MEMORY:
        throw out_of_host_memory(where);
    case CL_INVALID_PLATFORM:
        throw invalid_platform(where);
    case CL_BUILD_PROGRAM_FAILURE:
        throw build_program_failure(where);
    case CL_INVALID_KERNEL_ARGS:
        throw invalid_kernel_args(where);
    default:
        throw general_error("General error", where);
    }
}

} // end namespace signum
} // end namespace opengl

#endif /* SIGNUM_OPENCL_ERROR_HPP_ */
