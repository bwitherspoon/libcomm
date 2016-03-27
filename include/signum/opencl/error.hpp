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

struct opencl_error : public std::runtime_error
{
    opencl_error(const std::string &what) : runtime_error(what) { }
};

static inline void throw_on_error(const cl_int error)
{
    switch (error)
    {
    case CL_SUCCESS:
        return;
    case CL_PLATFORM_NOT_FOUND_KHR:
        return;
    case CL_INVALID_VALUE:
        throw opencl_error("Invalid value");
    case CL_OUT_OF_HOST_MEMORY:
        throw opencl_error("Host out of memory");
    case CL_INVALID_PLATFORM:
        throw opencl_error("Invalid platform");
    default:
        throw opencl_error("Unknown error");
    }
}

} // end namespace signum
} // end namespace opengl

#endif /* SIGNUM_OPENCL_ERROR_HPP_ */
