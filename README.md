# Affliction

For this project i will be following Vulkan api tutorial i found online, to hopefully get a jump start in graphics.

The tutorial can be found [here](https://vulkan-tutorial.com)

## Working with  github dependencies
TODO: update this to work with vcpkg 7zip files.

The dependencies are distributed in a precompiled fashion and are [documented here](deps/README.md)

## Download library dependencies

Make sure to manually install these on your system for optimal experience

* [Vulkan API v1.144](https://vulkan.lunarg.com/)
* [Doxygen](http://www.doxygen.nl/download.html)
* [Graphviz](http://www.graphviz.org/download/)

## VsCode CMake kit customization

When working with these tools it makes a lot of sense to specify a toolchain like so:

```json
  {
    "name": "MSVC x65",
    "visualStudio": "VisualStudio.16.0",
    "visualStudioArchitecture": "amd64",
    "preferredGenerator": {
      "name": "Visual Studio 17 2019",
      "platform": "x64"
    },
    "cmakeSettings": {
      "CMAKE_TOOLCHAIN_FILE": "C:/Users/djmit/vcpkg/scripts/buildsystems/vcpkg.cmake"
    }
  },
```

This will allow you to build 64 bit binary projects as well as specify location for `vcpkg` which is a very important tool. It provides cross platform package management for different toolchains (one of which is cmake).

## Building

In order to build this, please use `llvm-clang` and `ninja` do the following
however it does not link on windows for some reason 🤷‍♀️
So maybe figure out what works on windows rather than this?

```powershell
mkdir build && cd build
cmake -GNinja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_RC_COMPILER=llvm-rc ..
cmake --build .
```


```powershell
cmake -G"Visual Studio 16 2019" -Ax64 -DCMAKE_TOOLCHAIN_FILE=C:/Users/djmit/vcpkg/scripts/buildsystems/vcpkg.cmake ..
```

## Compiling shaders

TODO: build out a workshop for shaders as a subproject here.

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
