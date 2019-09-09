cmake_minimum_required(VERSION 3.15)

#Lulz
if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()


add_executable(hello helloworld.cpp)
target_compile_features(hello PRIVATE cxx_std_17)