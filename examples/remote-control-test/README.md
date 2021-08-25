# Remote Control Test
This example was written specifically for linux since it makes system calls to get user's keypresses.

## Cross-compiling for Jetson
If you want to compile this example for the Jetson on your personal computer, follow these instructions.

First, update these filepaths:
```c
# External header files
include_directories("/home/andrew/Documents/GitHub/Nemo/app")

# External source files
file(GLOB EXT_SOURCES "/home/andrew/Documents/GitHub/Nemo/app/*/*.cpp")
```
Then install arm 64 compilers:
```bash
sudo apt-get install g++-aarch64-linux-gnu
sudo apt-get install gcc-aarch64-linux-gnu
```
Then build the code:
```bash
mkdir cross-build/ && cd cross-build/
cmake ../ -DCMAKE_TOOLCHAIN_FILE=../../../app/cmake/jetson-toolchain.cmake
make
```
And you're done. You should now have a `serial-test` executable that won't run on your machine, but can run on the Jetson.

## Controls
- w --> start caudal fin
- s --> stop caudal fin
- up arrow --> pectoral fins straight ahead
- left arrow --> pectoral fins turn left
- right arrow --> pectoral fins turn right