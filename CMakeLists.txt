# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-format")
# MESSAGE( STATUS "this var key = ${CMAKE_CXX_FLAGS}.")
add_definitions(-Wno-format)
project(Gclock)
