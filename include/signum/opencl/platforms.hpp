/*
 * Copyright 2016 C. Brett Witherspoon
 */

#ifndef SIGNUM_OPENCL_PLATFORMS_HPP_
#define SIGNUM_OPENCL_PLATFORMS_HPP_

#include <string>
#include <map>
#include <vector>

#include <CL/cl.h>

namespace signum
{
namespace opencl
{
namespace platforms
{

using parameters = std::vector<std::map<std::string,std::string>>;

/**
 * Query the parameters of all the available platforms.
 *
 * \return the parameters of all the available platforms
 */
parameters query();

/**
 * Find the ID of a platform by name
 *
 * \param name the name of the platform to find
 */
cl_platform_id id(const std::string &name);

} // end namespace platforms
} // end namespace opencl
} // end namespace signum

#endif /* SIGNUM_OPENCL_PLATFORM_HPP_ */
