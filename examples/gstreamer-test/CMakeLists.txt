cmake_minimum_required(VERSION 3.16)
project(gstreamer_test)

message(WARNING "Andrew is awesome")

# Lib finder
find_package(Boost REQUIRED)
find_package(Threads REQUIRED)

# Internal header files
include_directories("./")

# Internal source files
file(GLOB SOURCES "*.cpp")
add_executable(gstreamer_test ${SOURCES})

# External header files
include_directories( ${Boost_INCLUDE_DIRS} )

# External libraries
target_link_libraries(gstreamer_test PRIVATE Threads::Threads)
