/*
 * Copyright 2016 C. Brett Witherspoon
 */

#ifndef SIGNUM_OPENCL_SESSION_HPP_
#define SIGNUM_OPENCL_SESSION_HPP_

#include <string>
#include <vector>

#include <CL/cl.h>

#include "signum/opencl/kernel.hpp"

namespace signum
{
namespace opencl
{

/**
 * An object for initialization, management, and cleanup of an OpenCL session.
 */
class session final
{
public:
    /**
     * Create an OpenCL session on the given platform. All devices on the
     * platform will be claimed.
     *
     * \param platform
     * \param program
     * \param kernels
     */
    explicit session(const std::string &platform, const std::string &program);

    //! A session cannot be copied
    session(const session &) = delete;

    //! A session cannot be moved
    session(const session &&other) = delete;

    //! A session cannot be copied
    session & operator=(const session &) = delete;

    //! A session cannot be moved
    session & operator=(const session &&other) = delete;

    //! Deconstruct a session
    ~session();

    /**
     * Construct a kernel object
     *
     * \param kernel the name of the kernel
     * \param flags the read or write flags for each argument of the kernel
     * \param device the device
     */
    kernel create_kernel(const std::string &name, unsigned int device = 0);

private:
    void create_context();
    void create_devices();
    void create_queues();
    void create_program(const std::string &program);

    cl_platform_id _platform;
    cl_context _context;
    std::vector<cl_device_id> _devices;
    std::vector<cl_command_queue> _queues;
    cl_program _program;
};

} // end namespace signum
} // end namespace opencl

#endif /* SIGNUM_OPENCL_SESSION_HPP_ */
