cmake_minimum_required(VERSION 2.8)
project(djikstra)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

set(SOURCE_EXE main.cpp)
set(SOURCE_LIB dijkstra.cpp graph.cpp)

add_library(djikstra STATIC ${SOURCE_LIB})
add_executable(main ${SOURCE_EXE})

target_link_libraries(main djikstra)
