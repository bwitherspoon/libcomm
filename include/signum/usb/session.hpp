/*
 * Copyright 2016 C. Brett Witherspoon
 *
 * This file is part of the signum library
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
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
