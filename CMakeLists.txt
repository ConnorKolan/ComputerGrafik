cmake_minimum_required(VERSION 3.0.0)
project(Monopoly VERSION 0.1.0 LANGUAGES C CXX)

add_executable(Monopoly src/main.cpp src/glad.c
        include/Shader.h
        include/stb_image.h)

find_library(GLFW_LIB glfw3 PATH ${CMAKE_SOURCE_DIR}/GLFW)
target_link_libraries(Monopoly ${CMAKE_SOURCE_DIR}/libs/libglfw3.a)

include_directories(${CMAKE_SOURCE_DIR}/include)

target_include_directories(Monopoly PUBLIC include/GLFW/include)
