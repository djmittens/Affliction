cmake_minimum_required(VERSION 3.15)

# find_package(Vulkan REQUIRED)

add_executable(werld src/main.cpp)

# target_compile_definitions(werld )

find_package(Boost 1.71 REQUIRED COMPONENTS log)


target_include_directories(werld PRIVATE Boost src )
target_link_libraries(werld PRIVATE Boost::log VulkanExperiment)

# target_include_directories(werld VulkanExperiment::VulkanExperiment)
# target_compile_definitions(werld )