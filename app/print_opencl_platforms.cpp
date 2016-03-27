/*
 * Copyright 2016 C. Brett Witherspoon
 */

#include <iostream>

#include <signum/opencl/error.hpp>
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

    // Print platform parameters
    std::cout << std::endl;
    for (const auto &plat: platforms)
    {
        for (const auto &param : plat)
        {
            std::cout << param.first << ": " << param.second << std::endl;
        }
        std::cout << std::endl;
    }

    return 0;
}
