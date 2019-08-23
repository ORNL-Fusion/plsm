cmake_minimum_required(VERSION 3.9)

project(PLSM CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")

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
target_sources(plsm INTERFACE ${PLSM_HEADERS})
target_include_directories(plsm INTERFACE "${PLSM_INCLUDE_DIR}")
target_link_libraries(plsm INTERFACE Kokkos)
target_compile_definitions(plsm INTERFACE TEST_SELECTOR)

option(PLSM_ENABLE_VTK "Use VTK to render 3D cases" OFF)
if(${PLSM_ENABLE_VTK})
    find_package(VTK REQUIRED
        COMPONENTS
        vtkRenderingOpenGL2
        vtkInteractionStyle
        NO_MODULE
    )
    # include(${VTK_USE_FILE})
    target_link_libraries(plsm INTERFACE ${VTK_LIBRARIES})
    target_include_directories(plsm SYSTEM INTERFACE ${VTK_INCLUDE_DIRS})
    target_compile_definitions(plsm INTERFACE
        ${VTK_DEFINITIONS}
        PLSM_ENABLE_VTK
    )
endif()

include(CTest)
if(${BUILD_TESTING})
    add_subdirectory(test)
endif()

find_package(Doxygen)
if(${DOXYGEN_FOUND})
    set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/doc")
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE README.md)
    set(DOXYGEN_GENERATE_TREEVIEW YES)
    set(DOXYGEN_TEMPLATE_RELATIONS YES)
    set(DOXYGEN_BUILTIN_STL_SUPPORT YES)
    set(DOXYGEN_EXTRACT_PRIVATE YES)
    set(DOXYGEN_SOURCE_BROWSER YES)
    set(DOXYGEN_INTERACTIVE_SVG YES)
    set(DOXYGEN_DISTRIBUTE_GROUP_DOC YES)

    doxygen_add_docs(plsmdoc ${CMAKE_CURRENT_LIST_DIR})
endif()
