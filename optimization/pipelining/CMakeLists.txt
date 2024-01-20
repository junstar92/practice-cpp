cmake_minimum_required(VERSION 3.14)
project(pipelining)

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

set(BENCHMARK_INCLUDE_DIR ${BENCHMARK_SOURCE_DIR}/include CACHE STRING "Benchmark include directory" FORCE)

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
            CXX_STANDARD 11
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

add_benchmark_target(
    superscalar_mbm
    ${CMAKE_SOURCE_DIR}/01_superscalar.cpp
)
add_benchmark_target(
    pipelining_mbm
    ${CMAKE_SOURCE_DIR}/02_pipelining.cpp
)
add_benchmark_target(
    branch_mbm
    ${CMAKE_SOURCE_DIR}/03_branch.cpp
)