/*
 *
 */

#include <iostream>
#include <chrono>
#include <cstdint>
#include <string>
#include <thread>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <comm/usb.hpp>
#include <comm/buffer.hpp>

int main (int argc, char *argv[])
{
    const uint16_t vid = 0x04b4;    // Netchip Technology, Inc
    const uint16_t pid = 0x00f1;    // Linux-USB "Gadget Zero"
    const std::string name = "usb";
    const int thresh = 1;
    std::atomic<size_t> sum(0);

    comm::usb::source src(vid, pid, comm::usb::endpoint::EP6_IN);

    std::thread usb([&]{ while(true) src(); });
    usb.detach();

    std::thread throughput([&]{
        while (true)
        {
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

    if (mkfifo(name.c_str(), 0666) == -1)
        throw std::runtime_error("Unable to create a named pipe");

    int fd = open(name.c_str(), O_WRONLY);
    if (fd == -1)
        throw std::runtime_error("Unable to open the named pipe");

    while (true)
    {
        out.wait(thresh);

        size_t len = out.size();

        if (write(fd, out.begin(), len) == -1)
            throw std::runtime_error("Failed to write to the named pipe");

        out.consume(len);

        sum += len;
    }

    return 0;
}
