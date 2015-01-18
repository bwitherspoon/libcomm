# C. Brett Witherspoon
#
# Find ZeroMQ
#
#   find_package(ZeroMQ)
#
# ZEROMQ_FOUND
# ZeroMQ_INCLUDE_DIRS
# ZeroMQ_LIBRARIES

find_package(PkgConfig)
pkg_check_modules(PC_ZeroMQ QUIET libzmq)

find_path(
    ZeroMQ_INCLUDE_DIRS zmq.h
    HINTS ${PC_ZeroMQ_INCLUDE_DIRS} ${PC_ZeroMQ_INCLUDEDIR}
)
find_library(
    ZeroMQ_LIBRARIES NAMES zmq libzmq
    HINTS ${PC_ZeroMQ_LIBRARY_DIRS} ${PC_ZeroMQ_LIBDIR}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    ZeroMQ
    DEFAULT_MSG
    ZeroMQ_INCLUDE_DIRS
    ZeroMQ_LIBRARIES
)

mark_as_advanced(
    ZeroMQ_INCLUDE_DIRS
    ZeroMQ_LIBRARIES
)
