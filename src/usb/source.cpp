/*
 * Copyright 2016 C. Brett Witherspoon
 */

#include <stdexcept>

#include <libusb.h>

#include "signum/usb/source.hpp"

namespace signum
{
namespace usb
{
source::source(uint16_t vid, uint16_t pid, endpoint ep)
    : d_endpoint(static_cast<unsigned char>(ep))
{
    if (libusb_init(NULL) < 0)
        throw std::runtime_error("Failed to initialize");

    libusb_set_debug(NULL, LIBUSB_LOG_LEVEL_INFO);

    d_handle = libusb_open_device_with_vid_pid(NULL, vid, pid);
    if (d_handle == NULL)
        throw::std::runtime_error("Failed to open device");

    const unsigned int max_packet_size = libusb_get_max_packet_size(libusb_get_device(d_handle), d_endpoint);

    // Lower 11 bits are the packet size (12-11 bits are packets per microframe_
    d_max_packet_size = max_packet_size & 0x7FF;

    d_buffer = signum::buffer::writer<unsigned char>(256 * d_max_packet_size);
}

source::~source()
{
    libusb_close(d_handle);

    libusb_exit(NULL);
}

void source::operator()()
{
    const int length = 64 * d_max_packet_size;

    int error;
    int transferred;

    d_buffer.wait(length);

    error = libusb_bulk_transfer(d_handle, d_endpoint, d_buffer.begin(), length, &transferred, 0);

    switch (error)
    {
        case 0:
            d_buffer.consume(transferred);
            break;
        case LIBUSB_ERROR_TIMEOUT:
            throw std::runtime_error("Timeout during bulk transfer");
        case LIBUSB_ERROR_PIPE:
            throw std::runtime_error("Endpoint halted during bulk transfer");
        case LIBUSB_ERROR_OVERFLOW:
            throw std::runtime_error("Overflow during bulk transfer");
        case LIBUSB_ERROR_NO_DEVICE:
            throw std::runtime_error("Device disconnected during bulk transfer");
        default:
            throw std::runtime_error("Error during bulk transfer");
    }
}

} // namespace usb
} // namespace signum
