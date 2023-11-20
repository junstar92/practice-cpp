cmake_minimum_required(VERSION 3.23)
project(accumulation_traits
    LANGUAGES CXX
)

add_executable(accum_trait)
set_target_properties(accum_trait
    PROPERTIES
        CXX_STANDARD 17
)
target_sources(accum_trait
    PRIVATE
        ${CMAKE_SOURCE_DIR}/main.cpp
)
target_include_directories(accum_trait 
    PRIVATE
        ${CMAKE_SOURCE_DIR}
)