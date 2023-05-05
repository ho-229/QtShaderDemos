cmake_minimum_required(VERSION 3.16)

project(QtShaderDemos VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_subdirectory(triangle)
add_subdirectory(rectangle)
add_subdirectory(texture)
