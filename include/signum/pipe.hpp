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

#ifndef SIGNUM_PIPE_HPP_
#define SIGNUM_PIPE_HPP_

#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

namespace signum
{

class pipe
{
public:
    pipe(const std::string &name)
    {
        // Try to open pipe (blocks until other end appears) or create it
        m_fd = open(name.c_str(), O_WRONLY);
        if (m_fd == -1)
        {
            if (mkfifo(name.c_str(), 0666) == -1)
                throw std::runtime_error("Unable to create named pipe: " + name);

            m_fd = open(name.c_str(), O_WRONLY);
            if (m_fd == -1)
                throw std::runtime_error("Unable to open named pipe: " + name);
        }
    }

    pipe(const pipe&) = delete;

    ~pipe()
    {
        close(m_fd);
    }

    pipe &operator=(const pipe&) = delete;

    bool operator==(const pipe &rhs) const { return m_fd == rhs.m_fd; }

    bool operator!=(const pipe &rhs) const { return !operator==(rhs); }

    operator int() const { return m_fd; }

    template<typename T>
    size_t write(T *data, size_t size)
    {
        ssize_t n = ::write(m_fd, data, size * sizeof(T));
        if (n == -1)
            throw std::runtime_error("Unable to write to named pipe");
        return n;
    }

private:
    int m_fd;
};

} // end namespace signum

#endif /* SIGNUM_PIPE_HPP_ */
