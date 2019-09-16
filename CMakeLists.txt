cmake_minimum_required(VERSION 3.15)

# what the heck man?
# set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
option(BUILD_DOC "Bould documentation" ON)

project(VulkanAffliction)

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
    -Wnull-dereference  # (only in GCC >= 6.0) warn if a null dereference is detected
    -Wuseless-cast  # (only in GCC >= 4.8)warn if you perform a cast to the same type
    -Wdouble-promotion #(GCC >= 4.6, Clang >= 3.8)  warn if float is implicit promoted to double
    -Wformat=2 # warn on security issues around functions that format output (ie printf)
    -Wlifetime # (only special branch of Clang currently) shows object lifetime issues
  )
endif()

find_package(Doxygen)
if(DOXYGEN_FOUND)
  #  our initial output configuration
  set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/../docs/Doxyfile.in)
  set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

  # Loading up the configuration file
  configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
  message("Doxygen build started")

  add_custom_target(doc_doxygen ALL
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM
  )

else(DOXYGEN_FOUND)
  message("[Doxygen](http://www.doxygen.nl/) needs to be installed to generate documentation.")
endif(DOXYGEN_FOUND)

# GCC ? 
# add_compile_options(-fdiagnostics-color=always)

# CLANG
# add_compile_options(-fcolor-diagnostics)

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