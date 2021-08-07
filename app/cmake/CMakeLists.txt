cmake_minimum_required(VERSION 3.16)
project(nemo)

message(WARNING "Andrew is awesome")

# External header files

# External libraries

# Internal header files
include_directories("../include")

# Internal source files
file(GLOB SOURCES "../src/*.cpp")

add_executable(nemo ${SOURCES})