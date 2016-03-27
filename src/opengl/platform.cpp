/*
 * Copyright 2016 C. Brett Witherspoon
 */

#include <algorithm>
#include <memory>
#include <utility>

#include "signum/opencl/error.hpp"
#include "signum/opencl/platform.hpp"

namespace
{

std::vector<cl_platform_id> get_ids()
{
    using signum::opencl::throw_on_error;

    cl_uint num_platforms;

    // Get the number of platforms
    throw_on_error(clGetPlatformIDs(0, nullptr, &num_platforms));

    // Get all the platforms if not empty
    std::vector<cl_platform_id> platforms(num_platforms);

    if (!platforms.empty())
        throw_on_error(clGetPlatformIDs(platforms.size(), platforms.data(), nullptr));

    return std::move(platforms);
}

std::string get_info(cl_platform_id id, cl_platform_info info)
{
    using signum::opencl::throw_on_error;

    // Get parameter size
    size_t size;
    throw_on_error(clGetPlatformInfo(id, info, 0, nullptr, &size));

    // Get parameter
    auto param = std::unique_ptr<char[]>(new char[size]);
    throw_on_error(clGetPlatformInfo(id, info, size, param.get(), nullptr));

    return std::string(param.get());
}

} // end namespace anonymous

namespace signum
{
namespace opencl
{
namespace platform
{

parameters query()
{
    static const std::map<cl_platform_info, std::string> mappings = {
            {CL_PLATFORM_PROFILE,    "Profile"   },
            {CL_PLATFORM_VERSION,    "Version"   },
            {CL_PLATFORM_NAME,       "Name"      },
            {CL_PLATFORM_VENDOR,     "Vender"    },
            {CL_PLATFORM_EXTENSIONS, "Extensions"}
    };

    platform::parameters result;

    // Query all platform attributes
    for (const auto &id : get_ids())
    {
        std::map<std::string,std::string> entry;

        for (const auto &map : mappings)
        {
            entry.insert(std::make_pair(map.second, get_info(id, map.first)));
        }

        result.push_back(std::move(entry));
    }

    return std::move(result);
}

cl_platform_id id(const std::string &name)
{
    auto ids = get_ids();

    auto it = std::find_if(ids.begin(), ids.end(), [&name](const auto &id) {
        return get_info(id, CL_PLATFORM_NAME) == name;
    });

    if (it == ids.end())
        throw opencl_error("Platform not found: " + name);

    return *it;
}

} // end namespace platform
} // end namespace signum
} // end namespace opencl
