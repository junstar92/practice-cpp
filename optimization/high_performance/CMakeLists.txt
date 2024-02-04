cmake_minimum_required(VERSION 3.14)
project(high_performance)

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
            CXX_STANDARD 17
            CXX_STANDARD_REQUIRED ON
    )
    target_compile_options(${_target_name}
        PRIVATE
            -g -O3 -Wall
    )
endmacro()

add_benchmark_target(
    vector_sort_mbm
    ${CMAKE_SOURCE_DIR}/01_vector_sort.cpp
)
add_target(
    rvo
    ${CMAKE_SOURCE_DIR}/02_rvo.cpp
)
add_benchmark_target(
    buffer
    ${CMAKE_SOURCE_DIR}/03_buffer.cpp
)
add_benchmark_target(
    branch
    ${CMAKE_SOURCE_DIR}/04_branch.cpp
)