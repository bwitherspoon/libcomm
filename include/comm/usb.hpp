/*
 * Copyright 2016 C. Brett Witherspoon
 */

#ifndef COMM_USB_HPP_
#define COMM_USB_HPP_

#include <cstdint>

#include "buffer.hpp"

struct libusb_device_handle;

namespace comm
{
namespace usb
{
enum class endpoint : unsigned char
{
    EP1_OUT = 0x01,
    EP2_OUT = 0x02,
    EP3_OUT = 0x03,
    EP4_OUT = 0x04,
    EP1_IN  = 0x81,
    EP2_IN  = 0x82,
    EP3_IN  = 0x83,
    EP4_IN  = 0x84
};

class source final
{
public:
    source(uint16_t vid, uint16_t pid, endpoint ep);

    ~source();

    template<typename T>
    comm::buffer::reader<T> make_reader() { return d_buffer.make_reader<T>(); }

    void operator()();

private:
    unsigned char d_endpoint;

    unsigned int d_max_packet_size;

    comm::buffer::writer<unsigned char> d_buffer;

    libusb_device_handle *d_handle;
};

} // namespace usb
} // namespace comm

#endif /* COMM_USB_HPP_ */
