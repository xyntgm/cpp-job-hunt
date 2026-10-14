cmake_minimum_required(VERSION 3.16)

project(devicehmi VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 REQUIRED COMPONENTS Widgets SerialPort)

# Qt 6.3+ 提供的标准设置：自动开启 AUTOMOC/AUTOUIC/AUTORCC
qt_standard_project_setup()

qt_add_executable(devicehmi
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/ITransport.h
    src/SerialTransport.cpp
    src/SerialTransport.h
    src/LoopbackTransport.cpp
    src/LoopbackTransport.h
    src/HexUtil.cpp
    src/HexUtil.h
)

target_include_directories(devicehmi PRIVATE src)

target_link_libraries(devicehmi PRIVATE
    Qt6::Widgets
    Qt6::SerialPort
)

if(MSVC)
    target_compile_options(devicehmi PRIVATE /W4)
else()
    target_compile_options(devicehmi PRIVATE -Wall -Wextra)
endif()
