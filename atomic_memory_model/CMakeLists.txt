cmake_minimum_required(VERSION 3.23)
project(atomic_memory
    LANGUAGES CXX
)

add_executable(${PROJECT_NAME})
set_target_properties(${PROJECT_NAME}
    PROPERTIES
        CXX_STANDARD 17
)
target_sources(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/main.cpp
)
target_include_directories(${PROJECT_NAME} 
    PRIVATE
        ${CMAKE_SOURCE_DIR}
)