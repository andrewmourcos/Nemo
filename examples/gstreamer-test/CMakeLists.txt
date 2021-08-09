cmake_minimum_required(VERSION 3.16)
project(gstreamer_test)

message(WARNING "Andrew is awesome")

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	# MacOS specifics for Kiran
	set(jsoncpp_DIR "/usr/local/Cellar/jsoncpp/1.9.4_1/lib")
endif()

# Lib finder
find_package(Boost REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(jsoncpp REQUIRED)

# Internal header files
include_directories("./")

# Internal source files
file(GLOB SOURCES "*.cpp")
add_executable(gstreamer_test ${SOURCES})

# External header files
include_directories( jsoncpp )

# External header files
include_directories( ${Boost_INCLUDE_DIRS} )

# External libraries
target_link_libraries(gstreamer_test PRIVATE Threads::Threads
											 OpenSSL::SSL 
											 OpenSSL::Crypto
											 jsoncpp)
