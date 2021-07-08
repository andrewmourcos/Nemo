# Nemo
CPP code that runs on-fish.
Status: nothing to run yet.

## Dependencies
- Recent version of CMake

## Building
There are two build options:
1) build binaries for local system
2) build binaries for jetson nano 2gb

Do all your testing locally and when it's time to test on the end device, cross-compile it with the instructions below and ping Andrew.

### Local build
In shell (make sure you're in this repo's root dir)
```
mkdir build && cd build
cmake ../cmake
make
```
After running `make`, the compiled binaries will be in your build/ folder.
If you make small code changes after building, just run `make clean` then `make` again.
If you make changes to the CMAKE files or just want a clean slate, delete the build/ folder and restart the commands above.

### Cross-compile for Jetson
```
mkdir build && cd build
cmake ../cmake \
		-DCMAKE_TOOLCHAIN_FILE=jetson-toolchain.cmake
make
```

## Project Structure Notes
All build-related files will be in `cmake/`.
All `*.c` or `*.cpp` files will be in `src/`.
All `*.h` or `*.hpp` files will be in `includes/`

Try to be consistent with tabs and whatnot. Cheers.