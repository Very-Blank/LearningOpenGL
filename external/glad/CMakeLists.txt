cmake_minimum_required(VERSION 3.22.1)

project(glad)

add_library(${PROJECT_NAME} include/glad.h src/glad.c)
target_include_directories(${PROJECT_NAME} PUBLIC include/)
target_link_directories(${PROJECT_NAME} PRIVATE src/)
