For this project i will be following Vulkan api tutorial i found online, to hopefully get a jump start in graphics.

The tutorial can be found [here](https://vulkan-tutorial.com)

## Downloading github dependencies
When downloading the project you can download all of the dependencies for it by doing:
```
git submodules update --init
```

## Compiling shaders
Can be done using `glslc` command that comes with vulkan api

```
shaders> glslc.exe --target-env=vulkan1.1 helloworld.vert -o helloworld.vert.spv
shaders> glslc.exe --target-env=vulkan1.1 helloworld.frag -o helloworld.frag.spv
```

## Random Notes
### Enable C++ 17 standard with clang
```
clang++ -std=c++17 helloworld.cpp
```