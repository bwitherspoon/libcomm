/*
 * Copyright 2016 C. Brett Witherspoon
 */

#ifndef SIGNUM_OPENCL_PLATFORM_HPP_
#define SIGNUM_OPENCL_PLATFORM_HPP_

#include <string>
#include <map>
#include <vector>

#include <CL/cl.h>

namespace signum
{
namespace opencl
{

class platform
{
public:
    using parameters = std::vector<std::map<std::string,std::string>>;

    /**
     * Query the parameters of the available platforms.
     *
     * \return the parameters of all the available platforms
     */
    static parameters query();

    /**
     * Construct a platform object with the given name.
     *
     * \param name the name of the platform
     */
    platform(const std::string &name);

private:
    std::vector<std::map<cl_platform_info,std::string>> infos();

    cl_platform_id _id;
};

} // end namespace signum
} // end namespace opengl

#endif /* SIGNUM_OPENCL_PLATFORM_HPP_ */
