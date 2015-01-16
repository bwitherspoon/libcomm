comm
====

Introduction
------------

This is a library for communication systems.

Dependencies
------------

This project requires your compiler support C++11.

The following libraries are required:

- CMake
- Boost

Building
--------

    git clone https://github.com/spoonb/comm.git
    cd comm
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=~/.local/ ..
    make
    make test
    make install

