/*
 * Copyright 2016 C. Brett Witherspoon
 */

#ifndef SIGNUM_USB_TRANSFER_HPP_
#define SIGNUM_USB_TRANSFER_HPP_

#include <libusb.h>

#include "signum/usb/device.hpp"

namespace signum {
namespace usb {

class transfer final
{
public:
    using callback = std::function<void(void *,size_t)>;

    transfer() : transfer_(nullptr) { }

    transfer(const device &dev, endpoint ep, uint8_t *buf, size_t len,
             callback cb = [](void *, size_t) { })
        : callback_(cb)
    {
        if (len % dev.max_packet_size(ep) != 0)
            throw std::invalid_argument("The buffer length must be a multiple"
                                        "of the endpoint's packet size");

        transfer_ = libusb_alloc_transfer(0);
        if (transfer_ == nullptr)
            throw std::runtime_error("Unable to allocate USB transfer");

        libusb_fill_bulk_transfer(transfer_,
                                  dev,
                                  static_cast<unsigned char>(ep),
                                  static_cast<unsigned char *>(buf),
                                  static_cast<int>(len),
                                  &on_transfer_complete,
                                  static_cast<void*>(this),
                                  0);
    }

    ~transfer()
    {
        if (transfer_ != nullptr)
            libusb_free_transfer(transfer_);
    }

    operator libusb_transfer*() const { return transfer_; }

    void set_callback(callback cb) { callback_ = cb; }

    void submit()
    {
        if (libusb_submit_transfer(transfer_) != LIBUSB_SUCCESS)
            throw std::runtime_error("Unable to submit USB transfer");
    }

private:
    static void on_transfer_complete(struct libusb_transfer *xfer)
    {
        auto self = static_cast<transfer*>(xfer->user_data);
        auto func = self->callback_;
        func(xfer->buffer, xfer->actual_length);
    }

    libusb_transfer *transfer_;

    callback callback_;
};

}
}



#endif /* SIGNUM_USB_TRANSFER_HPP_ */
