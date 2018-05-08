cmake_minimum_required(VERSION 3.0)
project(minisql)
set(CMAKE_CXX_STANDARD 14)

include_directories("${CMAKE_SOURCE_DIR}/include" "${CMAKE_SOURCE_DIR}/src")

file(GLOB SQL_SRC "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_executable(minisql ${SQL_SRC})