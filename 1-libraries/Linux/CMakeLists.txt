cmake_minimum_required(VERSION 3.28)
project(OS_2025)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(NumberLib)
add_subdirectory(VectorLib)
add_subdirectory(Main)

add_custom_target(build_all DEPENDS Main)