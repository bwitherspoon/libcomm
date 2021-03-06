Signum
=======

A library for signal processing and communication systems.

Dependencies
------------

Required to compile the library:

- [CMake](http://www.cmake.org/)
- [ZeroMQ](http://zeromq.org/) (optional)
- [libusb](http://libusb.info/) (optional)
- [OpenCL](https://www.khronos.org/opencl/) (optional)

Required for running the tests:

- [Boost.Test](http://www.boost.org/)

Required to build the documentation:

- [Doxygen](http://www.doxygen.org)
- [Graphviz](http://www.graphviz.org)

Getting Started
---------------

Clone the repository and build:

    git clone https://github.com/bwitherspoon/signum.git
    cd comm
    mkdir build
    cd build
    cmake ..
    make

Run the tests:

    make test

Install the library, headers, and executables:

    make install

Generate the documentation:

    make doc

