cmake_minimum_required(VERSION 3.15 FATAL_ERROR)


# what the heck man?
# set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
option(BUILD_DOC "Build documentation" ON)

project(Affliction)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# Disable some extraneous stuff
# set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
# set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
# set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
# set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)

# find_package(Vulkan REQUIRED)

set(MY_PROJECT_DEPENDENCIES glm glfw3 boost-log vulkan)

# shamelessly borrowing this setup for the toolchain from my 
if(NOT DEFINED ${CMAKE_TOOLCHAIN_FILE})
  if(NOT DEFINED ENV{VCPKG_ROOT})
    if(WIN32)
      set(VCPKG_ROOT $ENV{HOMEDRIVE}$ENV{HOMEPATH}/vcpkg)
    else()
      set(VCPKG_ROOT $ENV{HOME}/.vcpkg)
    endif()
  else()
    set(VCPKG_ROOT $ENV{VCPKG_ROOT})
  endif()

  if(NOT EXISTS ${VCPKG_ROOT})
    message("Cloning vcpkg in ${VCPKG_ROOT}")
    execute_process(COMMAND git clone https://github.com/Microsoft/vcpkg.git ${VCPKG_ROOT})
    # If a reproducible build is desired (and potentially old libraries are # ok), uncomment the
    # following line and pin the vcpkg repository to a specific githash.
    # execute_process(COMMAND git checkout 745a0aea597771a580d0b0f4886ea1e3a94dbca6 WORKING_DIRECTORY ${VCPKG_ROOT})
  else()
    # The following command has no effect if the vcpkg repository is in a detached head state.
    message("Auto-updating vcpkg in ${VCPKG_ROOT}")
    execute_process(COMMAND git pull WORKING_DIRECTORY ${VCPKG_ROOT})
  endif()

  if(NOT EXISTS ${VCPKG_ROOT}/README.md)
    message(FATAL_ERROR "***** FATAL ERROR: Could not clone vcpkg *****")
  endif()

  if(WIN32)
    set(BOOST_INCLUDEDIR ${VCPKG_ROOT}/installed/x86-windows/include)
    set(VCPKG_EXEC ${VCPKG_ROOT}/vcpkg.exe)
    set(VCPKG_BOOTSTRAP ${VCPKG_ROOT}/bootstrap-vcpkg.bat)
  else()
    set(VCPKG_EXEC ${VCPKG_ROOT}/vcpkg)
    set(VCPKG_BOOTSTRAP ${VCPKG_ROOT}/bootstrap-vcpkg.sh)
  endif()

  if(NOT EXISTS ${VCPKG_EXEC})
    message("Bootstrapping vcpkg in ${VCPKG_ROOT}")
    execute_process(COMMAND ${VCPKG_BOOTSTRAP} WORKING_DIRECTORY ${VCPKG_ROOT})
  endif()

  if(NOT EXISTS ${VCPKG_EXEC})
    message(FATAL_ERROR "***** FATAL ERROR: Could not bootstrap vcpkg *****")
  endif()

  set(CMAKE_TOOLCHAIN_FILE ${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake CACHE STRING "")

  message(STATUS "***** Checking project third party dependencies in ${VCPKG_ROOT} *****")
  # This only works with 64 bit builds for windows
  execute_process(COMMAND ${VCPKG_EXEC} install ${MY_PROJECT_DEPENDENCIES} --triplet x64-windows-static WORKING_DIRECTORY ${VCPKG_ROOT})
endif()

# add_subdirectory(extern/glfw)
# add_library(glfw::glfw ALIAS glfw)
# add_subdirectory(extern/glm)
# GOOFY AF, hack to make cross module dependencies work
# add_library(glm::glm ALIAS glm_static)

# Phew get to disable this after a bout of debugging.
# set(Boost_DEBUG ON)
# set(BOOST_ROOT "C:/lib/boost_1_71_0")
# set(BOOST_INCLUDEDIR "C:/lib/boost_1_71_0")
# set(BOOST_LIBRARYDIR "C:/lib/boost_1_71_0/stage/lib")
# set(Boost_ADDITIONAL_VERSIONS "1.71.0")
# set(Boost_DETAILED_FAILURE_MSG ON)
# set(Boost_USE_STATIC_LIBS ON)
# set(Boost_USE_MULTITHREADED ON)
# set(Boost_USE_STATIC_RUNTIME ON)


# find_package(Boost REQUIRED COMPONENTS log)
# find_package(glfw3 REQUIRED)
# find_package(glm REQUIRED)
# add_library(glm::glm ALIAS glm)
# add_library(glfw::glfw ALIAS glfw)

#Lulz
if(MSVC)
  add_compile_options(
    /permissive- # enforces standards conformance for msvc 
    /W4 /w14640 # turn up the # warnings.
    # /WX         # warnings as errors
    /w14242 # 'identfier': conversion from 'type1' to 'type1', possible loss of data
    /w14254 # 'operator': conversion from 'type1:field_bits' to 'type2:field_bits', possible loss of data
    /w14263 # 'function': member function does not override any base class virtual member function
    /w14265 # 'classname': class has virtual functions, but destructor is not virtual instances of this class may not be destructed correctly
    /w14287 # 'operator': unsigned/negative constant mismatch
    /we4289 # nonstandard extension used: 'variable': loop control variable declared in the for-loop is used outside the for-loop scope
    /w14296 # 'operator': expression is always 'boolean_value'
    /w14311 # 'variable': pointer truncation from 'type1' to 'type2'
    /w14545 # expression before comma evaluates to a function which is missing an argument list
    /w14546 # function call before comma missing argument list
    /w14547 # 'operator': operator before comma has no effect; expected operator with side-effect
    /w14549 # 'operator': operator before comma has no effect; did you intend 'operator'?
    /w14555 # expression has no effect; expected expression with side-effect
    /w14619 # pragma warning: there is no warning number 'number'
    /w14640 # Enable warning on thread un-safe static member initialization
    /w14826 # Conversion from 'type1' to 'type_2' is sign-extended. This may cause unexpected runtime behavior.
    /w14905 # wide string literal cast to 'LPSTR'
    /w14906 # string literal cast to 'LPWSTR'
    /w14928 # illegal copy-initialization; more than one user-defined conversion has been implicitly applied
  )
else()
  add_compile_options(
    -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic

    -Wall -Wextra # reasonable and standard
    -Wshadow # warn the user if a variable declaration shadows one from a parent context
    -Wnon-virtual-dtor # warn the user if a class with virtual functions has a non-virtual destructor. This helps catch hard to track down memory errors
    -Wold-style-cast # warn for c-style casts
    -Wcast-align # warn for potential performance problem casts
    -Wunused # warn on anything being unused
    -Woverloaded-virtual # warn if you overload (not override) a virtual function
    -Wpedantic # (all versions of GCC, Clang >= 3.2) warn if non-standard C++ is used
    -Wconversion # warn on type conversions that may lose data
    -Wsign-conversion  #(Clang all versions, GCC >= 4.3) warn on sign conversions
    # -Wmisleading-indentation (only in GCC >= 6.0) # warn if indentation implies blocks where blocks do not exist
    # -Wduplicated-cond (only in GCC >= 6.0) # warn if if / else chain has duplicated conditions
    # -Wduplicated-branches (only in GCC >= 7.0) # warn if if / else branches have duplicated code
    # -Wlogical-op  # (only in GCC)warn about logical operations being used where bitwise were probably wanted
    # -Wnull-dereference  # (only in GCC >= 6.0) warn if a null dereference is detected
    # -Wuseless-cast  # (only in GCC >= 4.8)warn if you perform a cast to the same type
    -Wdouble-promotion #(GCC >= 4.6, Clang >= 3.8)  warn if float is implicit promoted to double
    -Wformat=2 # warn on security issues around functions that format output (ie printf)
    # -Wlifetime # (only special branch of Clang currently) shows object lifetime issues
  )
endif()


include(GNUInstallDirs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")


# GCC ? 
# add_compile_options(-fdiagnostics-color=always)

# CLANG
# add_compile_options(-fcolor-diagnostics)

add_subdirectory(VulkanExperiment)
add_subdirectory(Werld)


set(DOC_DEST_DIR ${CMAKE_BINARY_DIR} CACHE PATH "Path to the doxygen output")
set(DOC_INPUT_ROOT_DIRS_LIST 
  ${CMAKE_SOURCE_DIR}/docs/front_page.md
  ${CMAKE_SOURCE_DIR}/VulkanExperiment/src 
  ${CMAKE_SOURCE_DIR}/VulkanExperiment/include 
  ${CMAKE_SOURCE_DIR}/Werld/src 
  # ${CMAKE_SOURCE_DIR}/Werld/include 
  CACHE PATH "Path from where to start to parse"
)
list(JOIN DOC_INPUT_ROOT_DIRS_LIST " " DOC_INPUT_ROOT_DIRS)

set(DOC_EXCLUDE_PATTERNS_DIRS ${CMAKE_BINARY_DIR} CACHE STRING "Regex patterns exclusion")
set(DOC_COMMON_IMG_PATH ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "Path where doxygen will find referenced images" )


include(UseDoxygenDoc)
add_doxygen_doc(
  DOXY_FILE
    ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in
  BUILD_DIR
    ${CMAKE_CURRENT_BINARY_DIR}
  TARGET_NAME
    docs
  COMMENT
    "HTML documentation"
)