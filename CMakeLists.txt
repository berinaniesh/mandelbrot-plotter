cmake_minimum_required(VERSION 3.10.0)
project(mandelbrot_plotter)

include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(mandelbrot_plotter src/mandelbrot.cpp)
target_link_libraries(mandelbrot_plotter ${CONAN_LIBS})
