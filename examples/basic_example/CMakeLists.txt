cmake_minimum_required(VERSION 3.17)
project(BasicExample)

set(CMAKE_CXX_STANDARD 17)

add_executable(BasicExample basic_example.cpp ../../list.hpp)