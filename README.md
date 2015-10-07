Comm
====

This is a library for communication systems.

Dependencies
------------

Required to compile and run tests:

- [CMake](http://www.cmake.org/)
- [Boost.Test](http://www.boost.org/)
- [ZeroMQ](http://zeromq.org/)

Required to build documentation:

- [Doxygen](http://www.doxygen.org)
- [Graphviz](http://www.graphviz.org)

Getting Started
---------------

Clone the repository and build in a subdirectory:

    git clone https://github.com/spoonb/comm.git
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

