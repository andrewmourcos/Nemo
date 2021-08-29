# Toolchain file to cross-compile foor Jetson Nano 2GB target
# TODO(andrew): finish jetson toolchain setup

set(JETSON_TARGET True) # CMakeLists will use this to determine what to compile

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR "aarch64")
set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)
#set(CMAKE_CUDA_COMPILER nvcc) # not yet
#set(CMAKE_CUDA_HOST_COMPILER aarch64-linux-gnu-gcc) # not yet
set(CMAKE_FIND_ROOT_PATH "/usr/aarch64-linux-gnu")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)