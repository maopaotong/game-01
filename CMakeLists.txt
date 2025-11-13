# game-01/CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(Game01 LANGUAGES CXX)

# 设置全局标准（可选）
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(MSVC)
    add_compile_options(/utf-8)
endif()
# 添加子项目（顺序很重要：先 framework，后 demo）
add_subdirectory(framework)
add_subdirectory(demo)
add_subdirectory(tools)