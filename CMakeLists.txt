cmake_minimum_required(VERSION 3.10)

project(mandelbrot)

find_library(gmp gmpxx sfml-window sfml-system sfml-graphics PATHS "/usr/lib/x86_64-linux-gnu")
#find_library(tbb PATHS "/usr/local/tbb-2019_U8/lib")

add_executable(${PROJECT_NAME} "main.cpp" "app.cpp" "baserender.cpp" "mandelbrotrender.cpp")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
find_package(OpenMP)
#set_property(TARGET ${PROJECT_NAME} PROPERTY OpenMP_CXX_FLAGS)
target_link_libraries(${PROJECT_NAME} gmp gmpxx sfml-window sfml-system sfml-graphics OpenMP::OpenMP_CXX)
