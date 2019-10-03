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

# function(create_test _name)
#     set(_test_name test_${_name})
#     add_executable(${_test_name} ${_test_name}.cpp)
#     target_link_libraries(${_test_name} PUBLIC
#         plsm
#     )
#     target_include_directories(${_test_name} PUBLIC
#         "${TESTING_SRC_DIR}/include"
#         "${TESTING_BIN_DIR}"
#     )
#     target_compile_options(${_test_name} PUBLIC
#         $<IF:$<CONFIG:Debug>,
#         -Wall
#         -Wextra
#         -Wpedantic
#         -Wnull-dereference
#         -Wunused
#         -Wshadow
#         -Wnon-virtual-dtor
#         -Woverloaded-virtual
#         -Wswitch-enum
#         -Wuseless-cast
#         -Wold-style-cast
#         -Wcast-align
#         -Wconversion
#         -Wsign-conversion
#         -Wdouble-promotion
#         -Wduplicated-cond
#         -Wduplicated-branches
#         -Wlogical-op
#         ,>
#     )
#     add_test(NAME ${_test_name} COMMAND ${_test_name})
# endfunction(create_test)

option(PLSM_ENABLE_VTK "Use VTK to render 3D cases" OFF)
if(${PLSM_ENABLE_VTK})
    find_package(VTK REQUIRED
        COMPONENTS
        vtkRenderingOpenGL2
        vtkInteractionStyle
        NO_MODULE
    )
endif()

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

    add_test(NAME ${_name} COMMAND ${_name})
endfunction(create_test)

macro(add_test_source _source_list _name)
    list(APPEND ${_source_list} test_${_name}.cpp)
endmacro(add_test_source)

add_test_source(PLSM_UNIT_TESTS CompactFlat)
add_test_source(PLSM_UNIT_TESTS Interval)
add_test_source(PLSM_UNIT_TESTS IntervalRange)
add_test_source(PLSM_UNIT_TESTS MultiIndex)
add_test_source(PLSM_UNIT_TESTS Region)
add_test_source(PLSM_UNIT_TESTS Segment)
add_test_source(PLSM_UNIT_TESTS SpaceVector)
# add_test_source(PLSM_UNIT_TESTS Subpaving)
add_test_source(PLSM_UNIT_TESTS Tile)
add_test_source(PLSM_UNIT_TESTS Zone)
add_test_source(PLSM_UNIT_TESTS Detectors)

create_test(UnitTests PLSM_UNIT_TESTS)

add_test_source(PLSM_BENCHMARKS Subpaving)

create_test(Benchmarks PLSM_BENCHMARKS)
target_compile_definitions(Benchmarks PUBLIC CATCH_CONFIG_ENABLE_BENCHMARKING)
