/*
 * Copyright 2016 C. Brett Witherspoon
 */

#include <stdexcept>

#include <libusb.h>

#include "comm/usb.hpp"

namespace comm
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

    d_max_packet_size = libusb_get_max_packet_size(libusb_get_device(d_handle), d_endpoint);

    d_buffer = comm::buffer::writer<unsigned char>(16 * d_max_packet_size);
}

source::~source()
{
    libusb_close(d_handle);

    libusb_exit(NULL);
}

void source::operator()()
{
    int error;
    int transferred;

    d_buffer.wait(4 * d_max_packet_size);

    error = libusb_bulk_transfer(d_handle, d_endpoint, d_buffer.begin(),
                                 4 * d_max_packet_size, &transferred, 0);

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
} // namespace comm
