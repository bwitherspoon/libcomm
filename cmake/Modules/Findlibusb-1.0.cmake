# C. Brett Witherspoon
#
# Find ZeroMQ
#
#   find_package(libusb-1.0)
#
# LIBUSB_FOUND
# LIBUSB_INCLUDE_DIRS
# LIBUSB_LIBRARIES

find_package(PkgConfig)
pkg_check_modules(PC_LIBUSB QUIET libusb-1.0)

find_path(
    LIBUSB_INCLUDE_DIRS libusb.h
    HINTS ${PC_LIBUSB_INCLUDE_DIRS} ${PC_LIBUSB_INCLUDEDIR}
)
find_library(
    LIBUSB_LIBRARIES NAMES usb-1.0 libusb-1.0
    HINTS ${PC_LIBUSB_LIBRARY_DIRS} ${PC_LIBUSB_LIBDIR}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    libusb-1.0
    DEFAULT_MSG
    LIBUSB_INCLUDE_DIRS
    LIBUSB_LIBRARIES
)

mark_as_advanced(
    LIBUSB_INCLUDE_DIRS
    LIBUSB_LIBRARIES
)
