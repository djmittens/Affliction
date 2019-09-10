cmake_minimum_required(VERSION 3.15)

project(VulkanAffliction)

#Lulz
if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra )
endif()

# Disable some extraneous stuff
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)

find_package(Vulkan REQUIRED)

add_subdirectory(extern/glfw)
add_library(glfw::glfw ALIAS glfw)
add_subdirectory(extern/glm)
# GOOFY AF, hack to make cross module dependencies work
add_library(glm::glm ALIAS glm_static)
add_subdirectory(VulkanExperiment)
add_subdirectory(Werld)

add_executable(hello helloworld.cpp)
target_compile_features(hello PRIVATE cxx_std_17)