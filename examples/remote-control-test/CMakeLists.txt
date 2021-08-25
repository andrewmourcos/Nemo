cmake_minimum_required(VERSION 3.16)
set (CMAKE_CXX_STANDARD 11)

project(serial_test)

message(WARNING "Andrew is awesome")

set(THREADS_PREFER_PTHREAD_FLAG ON)
set (CMAKE_CXX_STANDARD 11)

# Lib finder

# External header files
include_directories("/home/andrew/Documents/GitHub/Nemo/app")

# External source files
file(GLOB EXT_SOURCES "/home/andrew/Documents/GitHub/Nemo/app/*/*.cpp")

# Internal header files

# Internal source files
file(GLOB SOURCES "*.cpp")
add_executable(serial_test ${SOURCES} ${EXT_SOURCES})



# External libraries
# target_link_libraries(serial_test PRIVATE Threads::Threads
# 											 OpenSSL::SSL 
# 											 OpenSSL::Crypto)




