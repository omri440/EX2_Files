cmake_minimum_required(VERSION 3.30)
project(Ex2_C C)

set(CMAKE_C_STANDARD 11)

add_executable(Ex2_C main.c)

# Copy Bank1.txt and Bank2.txt to the build directory
configure_file(${CMAKE_SOURCE_DIR}/Bank1.txt ${CMAKE_BINARY_DIR}/Bank1.txt COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/Bank2.txt ${CMAKE_BINARY_DIR}/Bank2.txt COPYONLY)
