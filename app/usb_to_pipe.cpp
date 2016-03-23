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

    signum::util::set_realtime_priority();

    if (mkfifo(vm["name"].as<std::string>().c_str(), 0666) == -1)
        throw std::runtime_error("Unable to create a named pipe");

    int fd = open(vm["name"].as<std::string>().c_str(), O_WRONLY);
    if (fd == -1)
        throw std::runtime_error("Unable to open the named pipe");

    signum::usb::source src(vid, pid, static_cast<signum::usb::endpoint>(ep));

    std::thread usb([&]{ while(true) src(); });
    usb.detach();

    std::thread throughput([&]{
        std::cout << std::fixed << std::setprecision(2);

        while (true)
        {
            signum::util::set_normal_priority();

            sum = 0;

            auto start = std::chrono::steady_clock::now();

            std::this_thread::sleep_for(std::chrono::seconds(1));

            auto end = std::chrono::steady_clock::now();

            std::chrono::duration<double> diff = end - start;

            std::cout << "Effective rate: " << sum / 2.0 / diff.count() / 1e6 << " Msps" << std::endl;
        }
    });
    throughput.detach();

    auto out = src.make_reader<unsigned char>();

    while (true)
    {
        out.wait(2048);

        size_t len = out.size();

        if (write(fd, out.begin(), len) == -1)
            throw std::runtime_error("Failed to write to the named pipe");

        out.consume(len);

        sum += len;
    }

    return 0;
}
