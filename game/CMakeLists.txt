# 
cmake_minimum_required(VERSION 3.20)
project(game LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(fmt CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

# 不需要重复 find_package（除非有额外依赖）
file(GLOB_RECURSE  SOURCES "src/*.cpp")

add_executable(game ${SOURCES})

#add_compile_definitions(UNICODE _UNICODE)

# 链接到 framework
target_link_libraries(game PRIVATE framework imgui::imgui)        
#

target_include_directories(game PRIVATE include)

