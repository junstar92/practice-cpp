cmake_minimum_required(VERSION 3.14)
project(substring_sort)

set(CMAKE_BUILD_TYPE "Release")

if (APPLE)
    set(CMAKE_CXX_LINK_EXECUTABLE ${CMAKE_CXX_LINK_EXECUTABLE} "dsymutil <TARGET>")
endif()

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
    compare_mbm_1
    ${CMAKE_SOURCE_DIR}/01_compare_mbm.cpp
)

add_benchmark_target(
    compare_mbm_2
    ${CMAKE_SOURCE_DIR}/02_compare.cpp
    ${CMAKE_SOURCE_DIR}/02_compare_mbm.cpp
)

add_benchmark_target(
    compare_mbm_3
    ${CMAKE_SOURCE_DIR}/03_compare_mbm.cpp
)