cmake_minimum_required(VERSION 3.10)
project(FileScannerProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 查找Qt库
find_package(Qt5 REQUIRED COMPONENTS Core Concurrent Widgets Gui)

# 添加子项目
add_subdirectory(FileScanner)
add_subdirectory(FileScannerUI)