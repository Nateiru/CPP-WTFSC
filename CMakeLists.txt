cmake_minimum_required (VERSION 3.22)

project("WTFSC")

set(CMAKE_CXX_STANDARD 20)              # Compile as C++20.
set(CMAKE_CXX_STANDARD_REQUIRED ON)     # Require C++20 support.

# generate complie_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 设置【可执行文件】和【动态库】的输出位置 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(contrib)
add_subdirectory(src)
