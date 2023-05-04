cmake_minimum_required(VERSION 3.14)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt5 COMPONENTS Core Widgets OpenGL REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Widgets OpenGL REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        widget.cpp
        widget.h
)

qt_add_resources(PROJECT_SOURCES shader.qrc)

add_executable(apprectangle
    ${PROJECT_SOURCES}
)

target_link_libraries(apprectangle PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::OpenGL
)
