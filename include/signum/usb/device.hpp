/*
 * Copyright 2016 C. Brett Witherspoon
 */

#ifndef SIGNUM_USB_DEVICE_HPP_
#define SIGNUM_USB_DEVICE_HPP_

namespace signum {
namespace usb {

enum class endpoint : unsigned char
{
    OUT1 = 0x01,
    OUT2 = 0x02,
    OUT3 = 0x03,
    OUT4 = 0x04,
    OUT5 = 0x05,
    OUT6 = 0x06,
    IN1  = 0x81,
    IN2  = 0x82,
    IN3  = 0x83,
    IN4  = 0x84,
    IN5  = 0x85,
    IN6  = 0x86
};

class device final
{
public:
    device(const context &ctx, uint16_t vid, uint16_t pid)
    {
        handle_ = libusb_open_device_with_vid_pid(ctx, vid, pid);
        if (handle_ == nullptr)
            throw std::runtime_error("Failed to open USB device");

        libusb_set_auto_detach_kernel_driver(handle_, true);

        int error = libusb_claim_interface(handle_, 0);
        if (error != LIBUSB_SUCCESS)
        {
            libusb_close(handle_);
            throw std::runtime_error(libusb_strerror(static_cast<libusb_error>(error)));
        }
    }

    device(const device&) = delete;

    device(device &&other) : handle_(nullptr)
    {
        operator=(std::move(other));
    }

    ~device()
    {
        if (handle_ != nullptr)
        {
            libusb_release_interface(handle_, 0);
            libusb_close(handle_);
        }
    }

    device & operator=(const device &) = delete;

    device & operator=(device &&other)
    {
        if (handle_ != other.handle_)
        {
            handle_ = other.handle_;
            handle_ = nullptr;
        }
        return *this;
    }

    bool operator==(const device &rhs)
    {
        return handle_ == rhs.handle_;
    }

    bool operator!=(const device &rhs)
    {
        return !operator==(rhs);
    }

    operator libusb_device_handle *() const { return handle_; }

    unsigned int max_packet_size(endpoint ep) const
    {
        auto dev = libusb_get_device(handle_);
        auto endpoint = static_cast<unsigned char>(ep);
        auto size = libusb_get_max_packet_size(dev, endpoint);
        if (size < 0)
            throw std::runtime_error(libusb_strerror(static_cast<libusb_error>(size)));
        // Lower 11 bits are the packet size (12-11 bits are packets per microframe)
        return size & 0x7FF;
    }

private:
    libusb_device_handle *handle_;
};

} // end namespace usb
} // end namespace signum



#endif /* SIGNUM_USB_DEVICE_HPP_ */
