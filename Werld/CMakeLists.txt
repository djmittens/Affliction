cmake_minimum_required(VERSION 3.15)

# find_package(Vulkan REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)

add_executable(werld src/main.cpp)

# target_compile_definitions(werld )

# find_package(Boost 1.71 REQUIRED COMPONENTS log)


target_include_directories(werld PRIVATE src VulkanExperiment)
target_link_libraries(werld PRIVATE fmt::fmt spdlog::spdlog VulkanExperiment)

# target_include_directories(werld VulkanExperiment::VulkanExperiment)
# target_compile_definitions(werld )