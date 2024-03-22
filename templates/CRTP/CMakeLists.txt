cmake_minimum_required(VERSION 3.23)
project(crtp
    LANGUAGES CXX
)

set(CMAKE_BUILD_TYPE "Release")

find_package(Git REQUIRED)

include(FetchContent)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    benchmark
    GIT_REPOSITORY "https://github.com/google/benchmark"
    GIT_TAG main
    PATCH_COMMAND ""
)
FetchContent_MakeAvailable(benchmark)
FetchContent_GetProperties(benchmark
    SOURCE_DIR BENCHMARK_SOURCE_DIR
    BINARY_DIR BENCHMARK_BINARY_DIR
)

find_library(GPERFTOOLS_PROFILER_LIBRARY profiler)

macro(add_benchmark_target _target_name _file)
    add_executable(${_target_name} ${_file})

    foreach(var ${ARGN})
        target_sources(${_target_name}
        PRIVATE
            ${var}
        )
    endforeach()

    set_target_properties(${_target_name}
        PROPERTIES
            CXX_STANDARD 17
            CXX_STANDARD_REQUIRED ON
    )
    target_compile_options(${_target_name}
        PRIVATE
            -g -O3 -Wall
    )
    if (GPERFTOOLS_PROFILER_LIBRARY)
        target_link_options(${_target_name}
            PRIVATE
                -lprofiler
        )
    endif()
    target_link_libraries(${_target_name}
        PRIVATE
            benchmark::benchmark
    )
endmacro()

macro(add_target _target_name _file)
    add_executable(${_target_name} ${_file})

    foreach(var ${ARGN})
        target_sources(${_target_name}
        PRIVATE
            ${var}
        )
    endforeach()

    set_target_properties(${_target_name}
        PROPERTIES
            CXX_STANDARD 20
    )
    target_compile_options(${_target_name}
        PRIVATE
            -g -O3 -Wall
    )
endmacro()

add_benchmark_target(
    exam01
    ${CMAKE_SOURCE_DIR}/exam01.cpp
)
add_target(
    exam02
    ${CMAKE_SOURCE_DIR}/exam02.cpp
)
add_target(
    exam03
    ${CMAKE_SOURCE_DIR}/exam03.cpp
)
add_target(
    exam04
    ${CMAKE_SOURCE_DIR}/exam04.cpp
)
add_target(
    exam05
    ${CMAKE_SOURCE_DIR}/exam05.cpp
)
add_target(
    exam06
    ${CMAKE_SOURCE_DIR}/exam06.cpp
)
add_target(
    exam07
    ${CMAKE_SOURCE_DIR}/exam07.cpp
)
add_target(
    exam08
    ${CMAKE_SOURCE_DIR}/exam08.cpp
)
add_target(
    exam09
    ${CMAKE_SOURCE_DIR}/exam09.cpp
)