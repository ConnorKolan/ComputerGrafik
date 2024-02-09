cmake_minimum_required(VERSION 3.0.0)
project(ImGui VERSION 0.1.0 LANGUAGES C CXX)


include_directories(${CMAKE_SOURCE_DIR}/include)
link_directories(${CMAKE_SOURCE_DIR}/lib)

add_executable(Main src/main.cpp src/glad.c
                include/stb_images.h)

find_library(GLFW_LIB glfw3 PATH ${CMAKE_SOURCE_DIR}/GLFW)
target_link_libraries(Main ${CMAKE_SOURCE_DIR}/libs/libglfw3.a)
target_link_libraries(Main ${CMAKE_SOURCE_DIR}/libs/libassimp.dll.a)



