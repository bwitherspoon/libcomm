/*
 * Copyright 2016 C. Brett Witherspoon
 */

#ifndef SIGNUM_IP_UDP_HPP_
#define SIGNUM_IP_UDP_HPP_

#include <cstdint>
#include <cstring>
#include <stdexcept>

#include <sys/socket.h>
#include <arpa/inet.h>

namespace signum {
namespace ip {

class udp final
{
public:
    udp(const std::string &host, uint16_t port)
    {
        sock_ = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock_ < 0)
            throw std::runtime_error("Failed to create socket");

        std::memset(&addr_, 0, sizeof(addr_));
        addr_.sin_family = AF_INET;
        addr_.sin_addr.s_addr = inet_addr(host.c_str());
        addr_.sin_port = htons(port);
    }

    udp(const udp&) = delete;

    ~udp()
    {
        close(sock_);
    }

    udp & operator=(const udp&) = delete;

    size_t send(const void *buf, size_t len)
    {
        ssize_t rc = sendto(sock_, buf, len, 0, (struct sockaddr *) &addr_, sizeof(addr_));
        if (rc < 0)
            throw std::runtime_error("Failed to send on socket");
        return rc;
    }

private:
    int sock_;
    struct sockaddr_in addr_;
};

} // end namespace ip
} // end namespace signum

#endif /* SIGNUM_IP_UDP_HPP_ */
