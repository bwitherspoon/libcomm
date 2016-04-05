/*
 * Copyright 2016 C. Brett Witherspoon
 */

#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include <libusb.h>

#include <boost/program_options.hpp>

#include <signum/usb/context.hpp>
#include <signum/usb/device.hpp>
#include <signum/usb/session.hpp>
#include <signum/usb/transfer.hpp>
#include <signum/util/priority.hpp>

namespace po = boost::program_options;
namespace usb = signum::usb;

int main(int argc, char *argv[])
{
    uint16_t vid;
    uint16_t pid;
    unsigned char end;

    po::options_description desc("Supported options");
    desc.add_options()
        ("help,h", "print help message")
        ("vid,v", po::value<uint16_t>(&vid)->default_value(0x0525), "set vendor ID")
        ("pid,p", po::value<uint16_t>(&pid)->default_value(0xa4a0), "set product ID")
        ("end,e", po::value<unsigned char>(&end)->default_value(0x81), "set endpoint")
    ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << std::endl;
        return 1;
    }

    signum::util::set_realtime_priority();

    std::vector<unsigned char> data(2048);

    auto session = usb::session();
    auto context = session.get_context();
    auto device = session.create_device(0x0525, 0xa4a0);
    auto transfer = session.create_transfer(device, usb::endpoint::IN1,
                                            data.data(), data.size());
    transfer->set_callback([transfer](auto buff, auto size) { transfer->submit(); });
    transfer->submit();

    while (true)
    {
        std::this_thread::yield();
    }

    return 0;
}


