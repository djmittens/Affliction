cmake_minimum_required(VERSION 3.15)

set(VkE_VERSION "0.0.0")
project(VulkanExperiment VERSION ${VkE_VERSION} LANGUAGES CXX)

# if(MSVC)
#     add_compile_options(/W4)
# else ()
#     add_compile_options(-W -Wall -Werror)
# endif()

set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# find_package(glfw REQUIRED)
# find_package(glm REQUIRED)
find_package(Vulkan REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glfw3 3.3.0 CONFIG REQUIRED)
find_package(Boost 1.70.0 REQUIRED COMPONENTS log)

add_library(VulkanExperiment SHARED src/experiment.cpp src/logging.cpp)

# target_include_directories(VulkanExperiment INTERFACE ${PROJECT_INCLUDE_DIR})

target_include_directories(VulkanExperiment 
PUBLIC
"$<BUILD_INTERFACE:${PROJECT_INCLUDE_DIR}>"

PRIVATE
    ${Boost_INCLUDE_DIR}
    glfw
    glm
    Vulkan::Vulkan
    )

target_link_libraries(VulkanExperiment PRIVATE glfw glm Boost::log Vulkan::Vulkan)

# this is a hack for now until i learn more about the different C++ features and work through the compilation
target_compile_features(VulkanExperiment PUBLIC cxx_std_17)
# target_compile_features( VulkanExperiment PUBLIC cxx_nullptr cxx_lambdas cxx_inline_namespaces cxx_variadic_templates cxx_nonstatic_member_init)