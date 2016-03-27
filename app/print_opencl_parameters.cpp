/*
 * Copyright 2016 C. Brett Witherspoon
 */

#include <iostream>

#include <signum/opencl/error.hpp>
#include <signum/opencl/device.hpp>
#include <signum/opencl/platform.hpp>

using namespace signum;

int main(int argc, char *argv[])
{
    // Query platform parameters
    opencl::platform::parameters platforms;
    try
    {
        platforms = opencl::platform::query();
    }
    catch (opencl::opencl_error &e)
    {
        std::cout << "OpenCL: " << e.what();
    }

    if (platforms.empty())
    {
        std::cout << "OpenCL: No platforms found" << std::endl;
        return 1;
    }

    std::cout << std::endl << "Platforms: " << std::endl << std::endl;
    for (const auto &plat: platforms)
    {
        // Print platform parameters
        for (const auto &param : plat)
        {
            std::cout << param.first << ": " << param.second << std::endl;
        }

        // Query platform device parameters
        auto it = plat.find("Name");
        if (it == plat.end())
        {
            std::cout << "No devices on platform" << std::endl;
            continue;
        }

        opencl::device::parameters devices;
        try
        {
            devices = opencl::device::query(it->second);
        }
        catch (opencl::opencl_error &e)
        {
            std::cout << "OpenCL: " << e.what();
        }

        if (devices.empty())
        {
            std::cout << "OpenCL: No devices found" << std::endl;
            return 1;
        }

        // Print device parameters of platform
        std::cout << std::endl << "Devices: " << std::endl << std::endl;
        for (const auto &dev: devices)
        {
            for (const auto &param : dev)
            {
                std::cout << param.first << ": " << param.second << std::endl;
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    return 0;
}
