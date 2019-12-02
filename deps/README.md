# Working with third party dependencies.

To manage third party dependencies one needs to simply extract the `.7z` file inside of deps, 
and then reference the toolchain file thats made avaiable by vcpkg.

TODO: write instructions for using the third party libraries in the project.

## Exporting vcpkg dependencies
You can use the following command to create a 7zip file with all of the dependencies for this project
for windows.
Or you can use a different destination.

Either way move that file back into this repo.

TODO: managing dependencies even with vcpkg is a huge pain in the balls, i should automate it somehow.

```
vcpkg.exe export --7zip --triplet x64-windows glm fmt glfw3 vulkan
```