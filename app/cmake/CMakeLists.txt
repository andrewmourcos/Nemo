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
find_package(OpenCV REQUIRED)

find_package(PkgConfig)
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)

# Internal source files
file(GLOB SOURCES "../*.cpp")


# Internal header files
include_directories("../common")
include_directories("../fishIO")
include_directories("../socks")
include_directories("../actuators")
include_directories("../fishStream")
include_directories("../streaming")

# External header files
include_directories( ${Boost_INCLUDE_DIRS} )
include_directories( jsoncpp )
include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories( ${GLIB_INCLUDE_DIRS}
					 ${GSTREAMER_INCLUDE_DIRS} )

add_executable(nemo ${SOURCES} 
					"../socks/boost-sock.cpp"
					"../actuators/serial-actuators.cpp"
					"../fishIO/fishIO.cpp"
					"../fishStream/fishGST.cpp"
					"../streaming/gst-streamer.cpp")

# External libraries
link_directories(
        ${GLIB_LIBRARY_DIRS}
        ${GSTREAMER_LIBRARY_DIRS}
)
target_link_libraries(nemo PRIVATE Threads::Threads
								   OpenSSL::SSL 
								   OpenSSL::Crypto
								   jsoncpp
								   ${OpenCV_LIBS}
								   ${GSTREAMER_LIBRARIES})