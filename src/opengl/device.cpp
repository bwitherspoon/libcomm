/*
 * Copyright 2016 C. Brett Witherspoon
 */

#include <algorithm>
#include <memory>

#include "signum/opencl/device.hpp"
#include "signum/opencl/error.hpp"
#include "signum/opencl/platform.hpp"

namespace
{

std::vector<cl_device_id> get_ids(cl_platform_id platform)
{
    using signum::opencl::throw_on_error;

    cl_uint num_devices;

    // Get the number of devices
    throw_on_error(clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, nullptr, &num_devices));

    // Get all the devices if not empty
    std::vector<cl_device_id> devices(num_devices);

    if (!devices.empty())
        throw_on_error(clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, devices.size(), devices.data(), nullptr));

    return std::move(devices);
}

std::string get_info(cl_device_id device, cl_device_info param)
{
    using signum::opencl::throw_on_error;

    if (param != CL_DEVICE_NAME && param != CL_DEVICE_VENDOR)
        throw std::invalid_argument("Only device name and vendor parameters are supported");

    // Get parameter size
    size_t size;
    throw_on_error(clGetDeviceInfo(device, param, 0, nullptr, &size));

    // Get parameter value
    auto value = std::unique_ptr<char[]>(new char[size]);
    throw_on_error(clGetDeviceInfo(device, param, size, value.get(), nullptr));

    return std::string(value.get());
}

} // end namespace anonymous

namespace signum
{
namespace opencl
{
namespace device
{

parameters query(const std::string &plat)
{
    static const std::map<cl_device_info, std::string> mappings = {
            {CL_DEVICE_NAME,       "Name"      },
            {CL_DEVICE_VENDOR,     "Vender"    }
    };

    parameters params;

    for (const auto &id : get_ids(platform::id(plat)))
    {
        std::map<std::string,std::string> entry;

        for (const auto &map : mappings)
        {
            entry.insert(std::make_pair(map.second, get_info(id, map.first)));
        }

        params.push_back(std::move(entry));
    }

    return params;
}

cl_device_id id(const std::string &plat, const std::string &name)
{
    auto ids = get_ids(platform::id(plat));

    auto it = std::find_if(ids.begin(), ids.end(), [&](const auto &id) {
        return get_info(id, CL_DEVICE_NAME) == name;
    });

    if (it == ids.end())
        throw opencl_error("Device not found: " + name);

    return *it;
}

} // end namespace device
} // end namespace signum
} // end namespace opencl
