cmake_minimum_required(VERSION 3.16)
project(cpp_drills CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_compile_options(-Wall -Wextra)

add_library(drills_core src/drills.cpp)
target_include_directories(drills_core PUBLIC include)

add_executable(drills_check tests/test_drills.cpp)
target_link_libraries(drills_check PRIVATE drills_core)
set_target_properties(drills_check PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
