# plsm-config.cmake - package configuration file

get_filename_component(SELF_DIR "${CMAKE_CURRENT_LIST_FILE}" DIRECTORY)
include("${SELF_DIR}/plsm.cmake")

include(CMakeFindDependencyMacro)
list(APPEND CMAKE_PREFIX_PATH ${Kokkos_DIR})
find_dependency(Kokkos)
