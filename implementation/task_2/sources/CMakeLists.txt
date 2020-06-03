cmake_minimum_required(VERSION 2.8)
project(jacobi)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_COMPILER /usr/bin/mpicxx)
include_directories(/usr/include/mpi)

set(SOURCE_EXE main.cpp)
set(SOURCE_LIB jacobi.cpp)

add_library(jacobi STATIC ${SOURCE_LIB})
add_executable(main ${SOURCE_EXE})

target_link_libraries(main jacobi)
