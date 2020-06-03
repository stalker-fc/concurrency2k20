cmake_minimum_required(VERSION 2.8)
project(multiplication)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

set(SOURCE_EXE main.cpp)
set(SOURCE_LIB matrix.cpp)

add_library(matrix STATIC ${SOURCE_LIB})
add_executable(main ${SOURCE_EXE})

target_link_libraries(main matrix)
