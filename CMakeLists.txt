cmake_minimum_required(VERSION 3.9)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(plsm
    VERSION 2.0.0
    LANGUAGES CXX
)

list(APPEND CMAKE_PREFIX_PATH ${Kokkos_DIR})
find_package(Kokkos 3 REQUIRED)

set(PLSM_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include")
set(PLSM_HEADER_DIR "${PLSM_INCLUDE_DIR}/plsm")
set(PLSM_HEADERS
    ${PLSM_HEADER_DIR}/detail/KokkosExtension.h
    ${PLSM_HEADER_DIR}/detail/Refiner.h
    ${PLSM_HEADER_DIR}/detail/Refiner.inl
    ${PLSM_HEADER_DIR}/detail/SpaceVectorBase.h
    ${PLSM_HEADER_DIR}/detail/SubdivisionInfo.h
    ${PLSM_HEADER_DIR}/refine/BallDetector.h
    ${PLSM_HEADER_DIR}/refine/Detector.h
    ${PLSM_HEADER_DIR}/refine/MultiDetector.h
    ${PLSM_HEADER_DIR}/refine/PolylineDetector.h
    ${PLSM_HEADER_DIR}/refine/RegionDetector.h
    ${PLSM_HEADER_DIR}/CompactFlat.h
    ${PLSM_HEADER_DIR}/EnumIndexed.h
    ${PLSM_HEADER_DIR}/Interval.h
    ${PLSM_HEADER_DIR}/IntervalRange.h
    ${PLSM_HEADER_DIR}/MultiIndex.h
    ${PLSM_HEADER_DIR}/Region.h
    ${PLSM_HEADER_DIR}/Region.inl
    ${PLSM_HEADER_DIR}/Segment.h
    ${PLSM_HEADER_DIR}/SpaceVector.h
    ${PLSM_HEADER_DIR}/Subpaving.h
    ${PLSM_HEADER_DIR}/Subpaving.inl
    ${PLSM_HEADER_DIR}/Tile.h
    ${PLSM_HEADER_DIR}/Utility.h
    ${PLSM_HEADER_DIR}/Zone.h
)

set(PLSM_BINARY_INCLUDE_DIR ${CMAKE_BINARY_DIR}/include)
option(PLSM_USE_64BIT_INDEX_TYPE "" FALSE)
configure_file(${CMAKE_CURRENT_LIST_DIR}/config.h.in
    ${PLSM_BINARY_INCLUDE_DIR}/plsm/config.h
)

add_library(plsm INTERFACE)
target_include_directories(plsm INTERFACE
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${PLSM_INCLUDE_DIR}>
    $<BUILD_INTERFACE:${PLSM_BINARY_INCLUDE_DIR}>
)
target_link_libraries(plsm INTERFACE Kokkos::kokkos)

include(CTest)
add_subdirectory(test)

add_subdirectory(doc)

install(DIRECTORY include/plsm DESTINATION include)
install(FILES ${PLSM_BINARY_INCLUDE_DIR}/plsm/config.h DESTINATION include/plsm)
install(TARGETS plsm EXPORT plsm)
install(EXPORT plsm DESTINATION lib/plsm)

include(CMakePackageConfigHelpers)
configure_package_config_file(CMake/plsmConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/CMake/plsmConfig.cmake
    INSTALL_DESTINATION lib/plsm
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/CMake/plsmConfigVersion.cmake
    COMPATIBILITY SameMajorVersion
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/CMake/plsmConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/CMake/plsmConfigVersion.cmake
    DESTINATION lib/plsm
)
