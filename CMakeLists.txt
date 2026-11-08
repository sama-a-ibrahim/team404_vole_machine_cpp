cmake_minimum_required(VERSION 3.16)
project(VoleMachine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

file(GLOB CORE_SOURCES ${CMAKE_SOURCE_DIR}/*.cpp)
list(FILTER CORE_SOURCES EXCLUDE REGEX ".*/(Menu|main|A2-[^/]*)\\.cpp$")


add_executable(vole_console ${CORE_SOURCES} ${CMAKE_SOURCE_DIR}/Menu.cpp ${CMAKE_SOURCE_DIR}/main.cpp)
target_include_directories(vole_console PRIVATE ${CMAKE_SOURCE_DIR})


find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_executable(vole_gui
    ${CORE_SOURCES}
    gui/main_gui.cpp
    gui/MainWindow.cpp
    gui/MainWindow.h
)
target_include_directories(vole_gui PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/gui)
target_link_libraries(vole_gui PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)