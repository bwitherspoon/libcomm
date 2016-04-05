/*
 * Copyright 2016 C. Brett Witherspoon
 */

#ifndef SIGNUM_USB_CONTEXT_HPP_
#define SIGNUM_USB_CONTEXT_HPP_

#include <atomic>
#include <stdexcept>
#include <thread>

#include <libusb.h>

namespace signum {
namespace usb {

class context final
{
public:
    context()
    {
        int error = libusb_init(&context_);
        if (error != LIBUSB_SUCCESS)
            throw std::runtime_error(libusb_strerror(static_cast<libusb_error>(error)));

        set_debug(LIBUSB_LOG_LEVEL_INFO);
    }

    context(const context&) = delete;

    context(context &&other) : context_(nullptr)
    {
        operator=(std::move(other));
    }

    ~context()
    {
        if (context_ != nullptr)
            libusb_exit(context_);
    }

    context & operator=(const context &) = delete;

    context & operator=(context &&other)
    {
        if (context_ != other.context_)
        {
            context_ = other.context_;
            other.context_= nullptr;
        }
        return *this;
    }

    bool operator==(const context &rhs) const
    {
        return context_ == rhs.context_;
    }

    bool operator!=(const context &rhs) const
    {
        return !operator==(rhs);
    }

    operator libusb_context*() const { return context_; }

    void set_debug(int level)
    {
        libusb_set_debug(context_, level);
    }

private:
    libusb_context *context_;
};

} // end namespace usb
} // end namespace usb

#endif /* SIGNUM_USB_CONTEXT_HPP_ */
