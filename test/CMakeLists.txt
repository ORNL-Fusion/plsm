if(NOT ${BUILD_TESTING})
    return()
endif()

set(TESTING_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(TESTING_SRC_DIR ${CMAKE_CURRENT_LIST_DIR})

find_package(Boost REQUIRED COMPONENTS timer)

# Get Catch2 single-header
file(DOWNLOAD
    https://raw.githubusercontent.com/catchorg/Catch2/master/single_include/catch2/catch.hpp
    "${TESTING_BIN_DIR}/catch.hpp"
)

function(create_test _name)
    set(_test_name test_${_name})
    add_executable(${_test_name} ${_test_name}.cpp)
    target_link_libraries(${_test_name} PUBLIC
        plsm
        Boost::timer
    )
    target_include_directories(${_test_name} PUBLIC
        "${TESTING_SRC_DIR}/include"
        "${TESTING_BIN_DIR}"
    )
    target_compile_options(${_test_name} PUBLIC
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
    add_test(NAME ${_test_name} COMMAND ${_test_name})
endfunction(create_test)

create_test(CompactFlat)
create_test(Interval)
create_test(IntervalRange)
create_test(MultiIndex)
create_test(Region)
create_test(Segment)
create_test(SpaceVector)
create_test(Subpaving)
create_test(Tile)
create_test(Zone)
create_test(Detectors)
