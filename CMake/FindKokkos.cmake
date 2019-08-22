# Try to find Kokkos headers and libraries.
#
# Usage of this module as follows:
#
#     find_package(Kokkos [REQUIRED])
#
# Variables used by this module, they can change the default behaviour and need
# to be set before calling find_package:
#
#  Kokkos_DIR         Set this variable to the root of a Kokkos installation
#                      if the module has problems finding the
#                      proper installation path.
#
# Variables defined by this module:
#
#  Kokkos_FOUND            The system has Kokkos libraries and headers
#  Kokkos_LIBRARIES        Specification of Kokkos libraries to link against.
#  Kokkos_INCLUDE_DIRS     The location of Kokkos headers


find_path(Kokkos_DIR include/Kokkos_Core.hpp
    HINTS
    "${KOKKOS_DIR}"
    ENV KOKKOS_DIR
)

find_library(Kokkos_LIBRARIES
    NAMES libkokkos.a
    HINTS ${Kokkos_DIR}/lib
)

find_path(Kokkos_INCLUDE_DIRS
    NAMES Kokkos_Core.hpp
    HINTS ${Kokkos_DIR}/include
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Kokkos DEFAULT_MSG
    Kokkos_LIBRARIES
    Kokkos_INCLUDE_DIRS
)

# We need to know some of the configuration needed to support
# whichever Kokkos backend(s) we might use.
if(EXISTS ${Kokkos_DIR}/kokkos.cmake)
    # Older Kokkos revision (< 2.5)
    include("${Kokkos_DIR}/kokkos.cmake")
elseif(EXISTS ${Kokkos_DIR}/kokkos_generated_settings.cmake)
    # Newer Kokkos revision (2.5+)
    include("${Kokkos_DIR}/kokkos_generated_settings.cmake")
endif()

# * Serial, nothing else should be needed for our configuration.
if("${KOKKOS_GMAKE_DEVICES}" MATCHES "Serial")
    message(STATUS "Detected Kokkos support for Serial")
endif()

# For pthreads, we may need to add a pthreads library but shouldn't
# need to add anything to CXXFLAGS.
if("${KOKKOS_GMAKE_DEVICES}" MATCHES "Pthread")
    message(STATUS "Detected Kokkos support for Pthread")
endif()

# For OpenMP, we use CMake's support for figuring out what it
# needs to build OpenMP code rather than taking it from whatever
# configuration we used to configure Kokkos.
if("${KOKKOS_GMAKE_DEVICES}" MATCHES "OpenMP")
    message(STATUS "Detected Kokkos support for OpenMP.")
    find_package(OpenMP REQUIRED)
    link_libraries(OpenMP::OpenMP_CXX)
endif()

# For CUDA, we probably need to use whatever was used to configure Kokkos.
if("${KOKKOS_GMAKE_DEVICES}" MATCHES "Cuda")
    message(STATUS "Detected Kokkos support for CUDA")

    # We assume that the user is compiling with nvcc_wrapper,
    # which adds the appropriate linker flags and libraries for us.
endif()

# The flags we used when building Kokkos probably have a C++
# standard selection flag (e.g., -std=c++11).  We probably set this
# already ourself, so to avoid compiler warnings, we strip it.
list(FILTER KOKKOS_CXXFLAGS EXCLUDE REGEX "--std=[^ ]+")

# For std::threads (once it is supported), nothing should be
# needed beyond whatever C++ standards flag we give.

add_library(Kokkos INTERFACE)
target_link_libraries(Kokkos INTERFACE
    ${Kokkos_LIBRARIES}
    ${KOKKOS_EXTRA_LIBS}
)

target_include_directories(Kokkos SYSTEM INTERFACE ${Kokkos_INCLUDE_DIRS})
target_compile_options(Kokkos INTERFACE ${KOKKOS_CXXFLAGS})
