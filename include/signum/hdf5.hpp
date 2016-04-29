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

#ifndef SIGNUM_HDF5_HPP_
#define SIGNUM_HDF5_HPP_

#include <string>

#include <hdf5.h>

namespace signum
{

class hdf5
{
public:
    hdf5(const std::string &filename, const std::string &dataset)
        : m_offset(0)
    {
        m_file = H5Fopen(filename.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
        if (m_file < 0)
            throw std::runtime_error("Unable to open HDF5 file: " + filename);

        m_dataset = H5Dopen(m_file, dataset.c_str(), H5P_DEFAULT);
        if (m_dataset < 0)
            throw std::runtime_error("Unable to open HDF5 data set: " + dataset);

        m_dataspace = H5Dget_space(m_dataset);
        if (m_dataspace < 0)
            throw std::runtime_error("Unable to get HDF5 data space");

        m_length = size()[0];
    }

    hdf5(const hdf5&) = delete;

    ~hdf5()
    {
        H5Sclose(m_dataspace);
        H5Dclose(m_dataset);
        H5Fclose(m_file);
    }

    hdf5 &operator=(const hdf5&) = delete;

    int rank() const
    {
        return H5Sget_simple_extent_ndims(m_dataspace);
    }

    std::vector<hsize_t> size() const
    {
        std::vector<hsize_t> dims(rank());

        if (!dims.empty())
            H5Sget_simple_extent_dims(m_dataspace, &dims[0], nullptr);

        return std::move(dims);
    }

    template<typename T>
    size_t read(T *data, size_t size)
    {
        auto base = static_cast<void *>(data);

        size = std::min(m_length - m_offset, size);

        const hsize_t offset[] = { m_offset, 0 };
        const hsize_t count[] = { size, 1 };

        herr_t error;

        error = H5Sselect_hyperslab(m_dataspace, H5S_SELECT_SET, offset, nullptr, count, nullptr);
        if (error < 0)
            throw std::runtime_error("Unable to select data space hyperslab");

        hid_t memspace = H5Screate_simple(2, count, nullptr);

        hid_t memtype = H5Dget_type(m_dataset);

        error = H5Dread(m_dataset, memtype, memspace, m_dataspace, H5P_DEFAULT, base);
        if (error < 0)
        {
            H5Sclose(memspace);
            throw std::runtime_error("Unable to read HDF5 dataset");
        }

        H5Sclose(memspace);

        m_offset += size;

        return size;
    }

private:
    size_t m_offset;
    hid_t m_file;
    hid_t m_dataset;
    hid_t m_dataspace;
    size_t m_length;
};

}

#endif /* SIGNUM_HDF5_HPP_ */

