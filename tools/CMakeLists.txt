# game-01/demo/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(tools LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(fmt CONFIG REQUIRED)

# 不需要重复 find_package（除非有额外依赖）
file(GLOB_RECURSE  SOURCES "src/*.cpp")

add_executable(tools ${SOURCES})


# 链接 
target_link_libraries(tools PRIVATE framework)


target_include_directories(tools PRIVATE include)
