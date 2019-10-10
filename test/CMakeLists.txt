if(NOT ${BUILD_TESTING})
    return()
endif()

set(TESTING_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(TESTING_SRC_DIR ${CMAKE_CURRENT_LIST_DIR})

# Get Catch2 single-header
file(DOWNLOAD
    https://raw.githubusercontent.com/catchorg/Catch2/master/single_include/catch2/catch.hpp
    "${TESTING_BIN_DIR}/catch.hpp"
)

option(PLSM_ENABLE_VTK "Use VTK to render 3D cases" OFF)
if(${PLSM_ENABLE_VTK})
    find_package(VTK REQUIRED
        COMPONENTS
        vtkRenderingOpenGL2
        vtkInteractionStyle
        NO_MODULE
    )
endif()

macro(add_unittest_source _source_list _name)
    list(APPEND ${_source_list} unittest_${_name}.cpp)
endmacro(add_unittest_source)

macro(add_benchmark_source _source_list _name)
    list(APPEND ${_source_list} benchmark_${_name}.cpp)
endmacro(add_benchmark_source)

function(create_test _name _sources_list)
    add_executable(${_name} ${${_sources_list}} test_main.cpp)
    target_link_libraries(${_name} PUBLIC plsm)
    target_include_directories(${_name} PUBLIC
        "${TESTING_SRC_DIR}/include"
        "${TESTING_BIN_DIR}"
    )
    target_compile_options(${_name} PUBLIC
        $<IF:$<CONFIG:Debug>,
        -Wall
        -Wextra
        -Wpedantic
        -Wnull-dereference
        -Wunused
        -Wshadow
        -Wnon-virtual-dtor
        -Woverloaded-virtual
        -Wswitch-enum
        -Wuseless-cast
        -Wold-style-cast
        -Wcast-align
        -Wconversion
        -Wsign-conversion
        -Wdouble-promotion
        -Wduplicated-cond
        -Wduplicated-branches
        -Wlogical-op
        ,>
    )

    if(${PLSM_ENABLE_VTK})
        target_link_libraries(${_name} PUBLIC ${VTK_LIBRARIES})
        target_include_directories(${_name} SYSTEM PUBLIC ${VTK_INCLUDE_DIRS})
        target_compile_definitions(${_name} PUBLIC
            ${VTK_DEFINITIONS}
            PLSM_ENABLE_VTK
        )
    endif()

endfunction(create_test)

add_unittest_source(PLSM_UNIT_TESTS CompactFlat)
add_unittest_source(PLSM_UNIT_TESTS EnumIndexed)
add_unittest_source(PLSM_UNIT_TESTS Interval)
add_unittest_source(PLSM_UNIT_TESTS IntervalRange)
add_unittest_source(PLSM_UNIT_TESTS MultiIndex)
add_unittest_source(PLSM_UNIT_TESTS Region)
add_unittest_source(PLSM_UNIT_TESTS Segment)
add_unittest_source(PLSM_UNIT_TESTS SpaceVector)
add_unittest_source(PLSM_UNIT_TESTS Subpaving)
add_unittest_source(PLSM_UNIT_TESTS Tile)
add_unittest_source(PLSM_UNIT_TESTS Zone)
add_unittest_source(PLSM_UNIT_TESTS Detectors)
create_test(unittests PLSM_UNIT_TESTS)
add_test(NAME UnitTests COMMAND unittests)

add_benchmark_source(PLSM_BENCHMARKS Subpaving)
create_test(benchmarks PLSM_BENCHMARKS)
target_compile_definitions(benchmarks PUBLIC CATCH_CONFIG_ENABLE_BENCHMARKING)
# add_test(NAME Benchmarks COMMAND benchmarks)
