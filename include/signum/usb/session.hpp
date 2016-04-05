/*
 * Copyright 2016 C. Brett Witherspoon
 */

#ifndef SIGNUM_USB_SESSION_HPP_
#define SIGNUM_USB_SESSION_HPP_

#include <memory>

#include "signum/usb/context.hpp"
#include "signum/usb/device.hpp"
#include "signum/usb/transfer.hpp"

namespace signum {
namespace usb {

class session final
{
public:
    session() : context_(std::make_shared<context>())
    {
        auto ctx = context_;
        std::thread([ctx]() {
            for (;;)
            {
                try {
                    if (libusb_handle_events(*ctx) != LIBUSB_SUCCESS)
                    {
                        std::cerr << "Failed to handle USB events" << std::endl;
                        break;
                    }
                }
                catch (std::exception &e)
                {
                    std::cerr << e.what() << std::endl;
                    break;
                }
            }
        }).detach();
    }

    std::shared_ptr<context> get_context() const { return context_; }

    std::shared_ptr<device> create_device(uint16_t vid, uint16_t pid) const
    {
        return std::make_shared<device>(*context_, vid, pid);
    }

    std::shared_ptr<transfer> create_transfer(std::shared_ptr<device> dev,
                                              endpoint ep,
                                              uint8_t *buf,
                                              size_t len)
    {
        return std::make_shared<transfer>(*dev, ep, buf, len);
    }

private:
    std::shared_ptr<context> context_;
};

} // end namespace usb
} // end namespace signum

#endif /* SIGNUM_USB_SESSION_HPP_ */
