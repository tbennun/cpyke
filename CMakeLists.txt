cmake_minimum_required(VERSION 3.0)
project(cpyke)

add_subdirectory(pybind11)

include_directories(include)

add_library(cpyke SHARED "src/cpyke.cpp")
target_link_libraries(cpyke PRIVATE pybind11::embed)

add_executable(example example/example.cpp)
target_link_libraries(example cpyke)