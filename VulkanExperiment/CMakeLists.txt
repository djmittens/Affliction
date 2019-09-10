cmake_minimum_required(VERSION 3.15)

set(VkE_VERSION "0.0.0")
project(VulkanExperiment VERSION ${VkE_VERSION} LANGUAGES CXX)

if(MSVC)
    add_compile_options(/W4)
else ()
    add_compile_options(-W -Wall -Werror)
endif()

# find_package(glfw REQUIRED)
# find_package(glm REQUIRED)
find_package(Vulkan REQUIRED)

add_library(VulkanExperiment src/experiment.cpp)

target_include_directories(VulkanExperiment PUBLIC include/ glfw::glfw glm::glm ${Vulkan_INCLUDE_DIRS})
target_link_libraries(VulkanExperiment PUBLIC glfw::glfw glm::glm PRIVATE VK_USE_PLATFORM_WIN32_KHR)
target_compile_features( VulkanExperiment PUBLIC cxx_nullptr cxx_lambdas cxx_inline_namespaces cxx_variadic_templates cxx_nonstatic_member_init)