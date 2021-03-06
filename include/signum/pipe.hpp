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

#include <system_error>
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
    pipe()
        : m_name(), m_fd(-1)
    {
    }

    explicit pipe(const std::string &name, mode_t mode = 0660)
        : m_name(name)
    {
        struct stat st;

        if (stat(name.c_str(), &st) == -1)
        {
            if (errno != ENOENT)
                throw std::system_error(errno, std::system_category());
            else if (mkfifo(name.c_str(), mode) == -1)
                throw std::system_error(errno, std::system_category());
        }
        else if (!S_ISFIFO(st.st_mode))
        {
            throw std::runtime_error("Path " + name + " is not a FIFO");
        }

        m_fd = open(name.c_str(), O_WRONLY);
        if (m_fd == -1)
            throw std::system_error(errno, std::system_category());
    }

    pipe(const pipe&) = delete;

    pipe(pipe &&other)
        : m_name(other.m_name),
          m_fd(other.m_fd)
    {
        other.m_fd = -1;
        other.m_name = "";
    }

    ~pipe()
    {
        unlink();
        close(m_fd);
    }

    pipe &operator=(const pipe&) = delete;

    pipe &operator=(pipe &&other)
    {
        m_name = other.m_name;
        other.m_name = "";

        m_fd = other.m_fd;
        other.m_fd = -1;

        return *this;
    }

    bool operator==(const pipe &rhs) const { return m_fd == rhs.m_fd; }

    bool operator!=(const pipe &rhs) const { return !operator==(rhs); }

    operator int() const { return m_fd; }

    std::string name() const { return m_name; }

    void unlink() { ::unlink(m_name.c_str()); }

    template<typename T>
    size_t write(T *data, size_t size)
    {
        ssize_t n = ::write(m_fd, data, size * sizeof(T));
        if (n == -1)
            throw std::system_error(errno, std::system_category());
        return n;
    }

private:
    std::string m_name;
    int m_fd;
};

} // end namespace signum

#endif /* SIGNUM_PIPE_HPP_ */
