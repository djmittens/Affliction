# Affliction

For this project i will be following Vulkan api tutorial i found online, to hopefully get a jump start in graphics.

The tutorial can be found [here](https://vulkan-tutorial.com)

## Downloading github dependencies

When downloading the project you can download all of the dependencies for it by doing:

```powershell
git submodules update --init
```

## Download library dependencies

Make sure to manually install these on your system for optimal experience

* Vulkan API v1.144
* Doxygen

### Boost

TODO: There are special instructions for building boost libraries, for them to work, and there is a special way to be able to debug the cmake module for boost as well.  Look at the  [stackoverflow question for details](https://stackoverflow.com/questions/57870032/cmake-v3-15-3-cannot-find-boost-v1-71-0)

## Building

In order to build this, please use `llvm-clang` and `ninja` do the following
however it does not link on windows for some reason 🤷‍♀️

```powershell
mkdir build && cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_RC_COMPILER=llvm-rc ..
cmake --build .
```

## Compiling shaders

Can be done using `glslc` command that comes with vulkan api

```powershell
shaders> glslc.exe --target-env=vulkan1.1 helloworld.vert -o helloworld.vert.spv
shaders> glslc.exe --target-env=vulkan1.1 helloworld.frag -o helloworld.frag.spv
```

## Random Notes

### Enable C++ 17 standard with clang

```powershell
clang++ -std=c++17 helloworld.cpp
```
