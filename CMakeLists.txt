cmake_minimum_required(VERSION 3.30)

project(AdventOfCode2024)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-Wall)

add_subdirectory(day_1)
add_subdirectory(day_2)
add_subdirectory(day_3)
add_subdirectory(day_4)
