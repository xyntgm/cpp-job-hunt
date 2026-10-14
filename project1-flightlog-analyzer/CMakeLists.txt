cmake_minimum_required(VERSION 3.16)
project(flightlog_analyzer CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_compile_options(-Wall -Wextra)

# 核心库：解析 + 分析 + 报告
add_library(flightlog_core
    src/csv.cpp
    src/analysis.cpp
    src/report.cpp
)
target_include_directories(flightlog_core PUBLIC include)

# 命令行程序
add_executable(flightlog src/main.cpp)
target_link_libraries(flightlog PRIVATE flightlog_core)

# 单元测试
enable_testing()
add_executable(flightlog_tests tests/test_main.cpp)
target_link_libraries(flightlog_tests PRIVATE flightlog_core)
add_test(NAME core_tests COMMAND flightlog_tests)
set_tests_properties(core_tests PROPERTIES WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
