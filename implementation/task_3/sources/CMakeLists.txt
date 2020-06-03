cmake_minimum_required(VERSION 2.8)
project(sort)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_COMPILER /usr/bin/mpicxx)
include_directories(/usr/include/mpi)

set(SOURCES array.cpp quicksort.cpp)

add_library(quicksort STATIC ${SOURCES})

add_executable(main main.cpp)

target_link_libraries(main quicksort)
