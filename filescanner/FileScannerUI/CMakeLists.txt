cmake_minimum_required(VERSION 3.10)

get_filename_component(CURRENT_DIR_PATH "${CMAKE_CURRENT_LIST_DIR}" ABSOLUTE)
get_filename_component(CURRENT_DIR_NAME "${CURRENT_DIR_PATH}" NAME)
project(${CURRENT_DIR_NAME})

set(CMAKE_CXX_STANDARD 11)

if(MSVC)
    add_compile_options(/Zc:__cplusplus)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Widgets Gui REQUIRED)

file(GLOB UI_FILES "*.ui")
file(GLOB RCC_FILES "*.qrc")
file(GLOB HEADER_FILES "*.h")
file(GLOB CPP_FILES "*.cpp")

qt5_wrap_ui(UI_HEADERS ${UI_FILES})

source_group("Form Files" FILES ${UI_FILES})
source_group("Resource Files" FILES ${RCC_FILES})

add_executable(${PROJECT_NAME} WIN32 
    ${HEADER_FILES} 
    ${CPP_FILES} 
    ${UI_FILES} 
    ${RCC_FILES}
)

target_link_libraries(${PROJECT_NAME}
    Qt5::Widgets
    Qt5::Core
    Qt5::Gui
    FileScanner
)

if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:FileScanner>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMENT "Copying FileScanner.dll to output directory"
    )
endif()