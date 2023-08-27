cmake_minimum_required (VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)              # Compile as C++20.
set(CMAKE_CXX_STANDARD_REQUIRED ON)     # Require C++20 support.


add_subdirectory(contrib)
add_subdirectory(src)