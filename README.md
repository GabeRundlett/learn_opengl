# Learn OpenGL

This is a repository in which I intend to learn how to use OpenGL by experimenting with different concepts.

Currently, the most interesting concept I've created (at least in my opinion) is the raytraced voxel "game"

## Building

Dependencies:
- A C++ Compiler (MSVC, Clang, GCC)
- CMake
- the Conan C++ Package manager 
    - installs the required libraries through CMake

``` py
# create build dir
mkdir build
cd build
# configure cmake to release mode
cmake .. -DCMAKE_BUILD_TYPE=Release

# build all
cmake --build . -S ../ -B . 
# optionally, instead you can build just a single target:
cmake --build . -S ../ -B . --target raytraced_voxels

# back out of the build directory
cd ..
# run a target
./build/voxel_game/raytraced/Release/raytraced_voxels.exe
```
