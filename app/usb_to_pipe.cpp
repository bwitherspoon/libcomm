/*
 *
 */

#include <iostream>
#include <iomanip>
#include <chrono>
#include <cstdint>
#include <string>
#include <thread>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <boost/program_options.hpp>

#include <signum/usb.hpp>
#include <signum/buffer.hpp>
#include <signum/util/priority.hpp>

namespace po = boost::program_options;

// These calls block until reader is available
int create_named_pipe(std::string name)
{
    // Try to open the pipe first in case it exists
    int fd = open(name.c_str(), O_WRONLY);

    // If open failed try to create the pipe
    if (fd == -1)
    {
        if (mkfifo(name.c_str(), 0666) == -1)
            throw std::runtime_error("Unable to create a named pipe");

        fd = open(name.c_str(), O_WRONLY);
        if (fd == -1)
            throw std::runtime_error("Unable to open the named pipe");
    }

    return fd;
}

int main (int argc, char *argv[])
{
    uint16_t vid;
    uint16_t pid;
    unsigned char ep;

    po::options_description desc("Supported options");
    desc.add_options()
        ("help,h", "print help message")
        ("vid,v", po::value<uint16_t>(&vid)->default_value(0x0525), "set vendor ID")
        ("pid,p", po::value<uint16_t>(&pid)->default_value(0xa4a0), "set product ID")
        ("ep,e", po::value<unsigned char>(&ep)->default_value(0x81), "set endpoint")
        ("name,n", po::value<std::string>()->default_value("/tmp/usb_to_pipe"), "set name of pipe")
    ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << std::endl;
        return 1;
    }

    std::atomic<size_t> sum(0);

    int fd = create_named_pipe(vm["name"].as<std::string>());

    signum::util::set_realtime_priority();

    signum::usb::source src(vid, pid, static_cast<signum::usb::endpoint>(ep));

    std::thread usb([&]{ while(true) src(); });
    usb.detach();

    std::thread throughput([&]{
        const char rot[] = { '-', '\\', '|', '/'};
        int idx = 0;

        signum::util::set_normal_priority();

        std::cout << std::fixed << std::setprecision(2) << std::endl;

        while (true)
        {
            sum = 0;

            auto start = std::chrono::steady_clock::now();
            std::this_thread::sleep_for(std::chrono::seconds(1));
            auto end = std::chrono::steady_clock::now();
            std::chrono::duration<double> diff = end - start;

            auto rate = sum / 2.0 / diff.count() / 1e6;

            std::cout << "\r" << rot[idx] << " Rate: " << rate << " Msps " << rot[idx] << std::flush;

            idx = (idx + 1) % 4;
        }
    });
    throughput.detach();

    auto out = src.make_reader<unsigned char>();

    while (true)
    {
        out.wait(4096);

        size_t len = out.size();

        if (write(fd, out.begin(), len) == -1)
            throw std::runtime_error("Failed to write to the named pipe");

        out.consume(len);

        sum += len;
    }

    return 0;
}
