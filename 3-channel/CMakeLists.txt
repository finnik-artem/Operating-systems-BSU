cmake_minimum_required(VERSION 3.28)
project(homework-3 VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

add_executable(channel main.cpp
            buffered_channel.h)
