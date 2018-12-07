# the minimum version of CMake required
cmake_minimum_required (VERSION 3.2)

project(Foo)

include_directories(include)

add_subdirectory(src)
add_subdirectory(examples)
add_subdirectory(tests)
