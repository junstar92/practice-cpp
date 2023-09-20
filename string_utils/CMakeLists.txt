cmake_minimum_required(VERSION 3.23)
project(string_utils
    LANGUAGES CXX
)

add_executable(strutils)
set_target_properties(strutils
    PROPERTIES
        CXX_STANDARD 17
)
target_sources(strutils
    PRIVATE
        ${CMAKE_SOURCE_DIR}/main.cpp
)
target_include_directories(strutils 
    PRIVATE
        ${CMAKE_SOURCE_DIR}
)