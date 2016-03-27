# C. Brett Witherspoon
#
# Find OpenCL
#
#   find_package(OpenCL)
#
# OPENCL_FOUND
# OpenCL_INCLUDE_DIRS
# OpenCL_LIBRARIES

find_package(PkgConfig)
pkg_check_modules(PC_OpenCL QUIET OpenCL)

find_path(
    OpenCL_INCLUDE_DIRS CL/opencl.h
    HINTS ${PC_OpenCL_INCLUDE_DIRS} ${PC_OpenCL_INCLUDEDIR}
)
find_library(
    OpenCL_LIBRARIES NAMES OpenCL
    HINTS ${PC_OpenCL_LIBRARY_DIRS} ${PC_OpenCL_LIBDIR}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    OpenCL
    DEFAULT_MSG
    OpenCL_INCLUDE_DIRS
    OpenCL_LIBRARIES
)

mark_as_advanced(
    OpenCL_INCLUDE_DIRS
    OpenCL_LIBRARIES
)
