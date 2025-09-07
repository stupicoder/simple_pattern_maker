cmake_minimum_required(VERSION 3.10.0)
project(basicAA VERSION 0.1.0 LANGUAGES CXX)

# C++ 표준 설정 (예: C++23)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(basicAA sources/main.cpp)
