cmake_minimum_required(VERSION 3.1...3.29)

project(engine-kernel VERSION 0.1.0 LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${CMAKE_SOURCE_DIR}/inc/) 

file(GLOB_RECURSE SOURCES "src/*.c")
add_executable(engine-kernel ${SOURCES})
set_property(TARGET engine-kernel PROPERTY C_STANDARD 11)

