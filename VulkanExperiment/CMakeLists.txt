cmake_minimum_required(VERSION 3.15)

set(VkE_VERSION "0.0.0")
project(VulkanExperiment VERSION ${VkE_VERSION} LANGUAGES CXX)

if(MSVC)
    add_compile_options(/W4)
else ()
    add_compile_options(-W -Wall -Werror)
endif()

set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# find_package(glfw REQUIRED)
# find_package(glm REQUIRED)
find_package(Vulkan REQUIRED)

add_library(VulkanExperiment src/experiment.cpp )

target_include_directories(VulkanExperiment PUBLIC 
    glfw::glfw 
    glm::glm 
    ${Vulkan_INCLUDE_DIRS} 
    ${PROJECT_INCLUDE_DIR})
    # "$<BUILD_INTERFACE:${PROJECT_INCLUDE_DIR}>")
target_link_libraries(VulkanExperiment PUBLIC glfw::glfw glm::glm ${Vulkan_LIBRARIES})
# this is a hack for now until i learn more about the different C++ features and work through the compilation
target_compile_features(VulkanExperiment PUBLIC cxx_std_17)
# target_compile_features( VulkanExperiment PUBLIC cxx_nullptr cxx_lambdas cxx_inline_namespaces cxx_variadic_templates cxx_nonstatic_member_init)