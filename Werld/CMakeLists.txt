cmake_minimum_required(VERSION 3.15)

find_package(Vulkan REQUIRED)

add_executable(werld src/main.cpp)

# target_compile_definitions(werld )


target_link_libraries(werld PRIVATE VulkanExperiment)

# target_include_directories(werld VulkanExperiment::VulkanExperiment)

target_include_directories(werld PRIVATE src )

# target_compile_definitions(werld )