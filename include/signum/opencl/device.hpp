/*
 * Copyright 2016 C. Brett Witherspoon
 */

#ifndef SIGNUM_OPENCL_DEVICE_HPP_
#define SIGNUM_OPENCL_DEVICE_HPP_

#include <map>
#include <string>
#include <vector>

#include <CL/cl.h>

namespace signum
{
namespace opencl
{
namespace device
{

using parameters = std::vector<std::map<std::string,std::string>>;

parameters query(const std::string &platform);

cl_device_id id(const std::string *platform, const std::string &name);

} // end namespace device
} // end namespace signum
} // end namespace opencl

#endif /* SIGNUM_OPENCL_DEVICE_HPP_ */
