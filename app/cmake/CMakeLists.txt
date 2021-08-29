cmake_minimum_required(VERSION 3.16)
project(nemo)

message(WARNING "Andrew is awesome")

set(THREADS_PREFER_PTHREAD_FLAG ON)
set (CMAKE_CXX_STANDARD 11)

# Lib finder
find_package(Boost REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(jsoncpp REQUIRED)

# Internal source files
file(GLOB SOURCES "../*.cpp")


# Internal header files
include_directories("../common")
include_directories("../fishIO")
include_directories("../socks")
include_directories("../actuators")

# External header files
include_directories( ${Boost_INCLUDE_DIRS} )
include_directories( jsoncpp )

add_executable(nemo ${SOURCES} 
					"../socks/boost-sock.cpp"
					"../actuators/serial-actuators.cpp"
					"../fishIO/fishIO.cpp")

# External libraries
target_link_libraries(nemo PRIVATE Threads::Threads
								   OpenSSL::SSL 
								   OpenSSL::Crypto
								   jsoncpp)