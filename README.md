Comm
====

This is a library for communication systems.

Dependencies
------------

Required external dependencies:

- [CMake](http://www.cmake.org/)
- [Boost.Test](http://www.boost.org/)
- [ZeroMQ](http://zeromq.org/)

Getting Started
---------------

Clone the repository and compile in a subdirectory:

    git clone https://github.com/spoonb/comm.git
    cd comm
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make

Run the tests:

    make test

Currently no files are installed and only the tests can be run.

Documentation
-------------

Run [Doxygen](http://www.doxygen.org) from the *doc* subdirectory:

    cd doc
    doxygen

