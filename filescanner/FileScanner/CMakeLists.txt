cmake_minimum_required(VERSION 3.10)
project(FileScanner LANGUAGES CXX)

set(SOURCES
    FileScanner.cpp
)

set(HEADERS
    FileScanner.h
)

# 设置自动生成目录
set(CMAKE_AUTOGEN_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/autogen)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 添加库目标
add_library(FileScanner SHARED ${SOURCES} ${HEADERS})

# 仅在新版CMake中设置这些属性
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.17)
    set_target_properties(FileScanner PROPERTIES
        AUTOMOC_MACRO_NAMES "Q_OBJECT;Q_GADGET"
        # 移除非法的 AUTOMOC_DEPEND_FILTERS 设置
    )
endif()

target_compile_definitions(FileScanner PRIVATE FILESCANNER_LIBRARY)

target_link_libraries(FileScanner
    Qt5::Core
    Qt5::Concurrent
)