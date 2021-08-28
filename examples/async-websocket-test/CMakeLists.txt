cmake_minimum_required(VERSION 3.16)
set (CMAKE_CXX_STANDARD 11)

project(websocket_test)

message(WARNING "Andrew is awesome")

set(THREADS_PREFER_PTHREAD_FLAG ON)
set (CMAKE_CXX_STANDARD 11)

# Lib finder
find_package(Boost REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

# Internal header files

# Internal source files
file(GLOB SOURCES "*.cpp")
add_executable(websocket_test ${SOURCES})

# External header files
include_directories( ${Boost_INCLUDE_DIRS} )

# External libraries
target_link_libraries(websocket_test PRIVATE Threads::Threads
											 OpenSSL::SSL 
											 OpenSSL::Crypto)




