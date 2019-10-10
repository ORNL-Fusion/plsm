cmake_minimum_required(VERSION 3.9)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(ENV{KOKKOS_CMAKE_CXX_STANDARD} ${CMAKE_CXX_STANDARD})
project(PLSM CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")

list(APPEND CMAKE_PREFIX_PATH ${Kokkos_DIR})
find_package(Kokkos REQUIRED)

set(PLSM_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include")
set(PLSM_HEADER_DIR "${PLSM_INCLUDE_DIR}/plsm")
set(PLSM_HEADERS
    ${PLSM_HEADER_DIR}/detail/KokkosExtension.h
    ${PLSM_HEADER_DIR}/detail/Refiner.h
    ${PLSM_HEADER_DIR}/detail/Refiner.inl
    ${PLSM_HEADER_DIR}/detail/SpaceVectorBase.h
    ${PLSM_HEADER_DIR}/refine/BallDetector.h
    ${PLSM_HEADER_DIR}/refine/Detector.h
    ${PLSM_HEADER_DIR}/refine/PolylineDetector.h
    ${PLSM_HEADER_DIR}/refine/RegionDetector.h
    ${PLSM_HEADER_DIR}/CompactFlat.h
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

add_library(plsm INTERFACE)
target_sources(plsm INTERFACE
    $<BUILD_INTERFACE:${PLSM_HEADERS}>)
target_include_directories(plsm INTERFACE
    $<BUILD_INTERFACE:${PLSM_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>
)
# target_link_libraries(plsm INTERFACE $<BUILD_INTERFACE:Kokkos::kokkos>)
target_link_libraries(plsm INTERFACE Kokkos::kokkos)
###
###
target_compile_definitions(plsm INTERFACE TEST_SELECTOR)
###
###

include(CTest)
add_subdirectory(test)

add_subdirectory(doc)

install(DIRECTORY include DESTINATION .)
install(TARGETS plsm EXPORT plsm)
install(FILES CMake/plsm-config.cmake DESTINATION lib/plsm)
install(EXPORT plsm DESTINATION lib/plsm)
