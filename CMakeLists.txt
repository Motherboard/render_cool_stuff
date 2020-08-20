cmake_minimum_required(VERSION 2.8)

project(mandelbrot)

find_library(gmp gmpxx sfml-window sfml-system sfml-graphics PATHS "/usr/lib/x86_64-linux-gnu")

add_executable(${PROJECT_NAME} "main.cpp")
target_link_libraries(${PROJECT_NAME} gmp gmpxx sfml-window sfml-system sfml-graphics)
