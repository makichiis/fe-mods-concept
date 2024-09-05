cmake_minimum_required(VERSION 3.1...3.29)

project(ecs VERSION 0.1.0 LANGUAGES CXX)

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_library(ecs SHARED ${SOURCES})

