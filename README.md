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

Clone the repository and compile:

```shell
git clone https://github.com/spoonb/comm.git
cd comm
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

Run the tests:

```shell
make test
```

Currently no files are installed only tests can be run.

Documentation
-------------

Doxygen is required to generate the documentation.

Run Doxygen from the documention subdirectory:

```shell
cd doc
doxygen
```
