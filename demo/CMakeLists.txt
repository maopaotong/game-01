# game-01/demo/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(fmt CONFIG REQUIRED)
find_package(OGRE CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)

# 不需要重复 find_package（除非有额外依赖）
file(GLOB_RECURSE  SOURCES "src/*.cpp")

add_executable(demo ${SOURCES})


# 链接到 framework
target_link_libraries(demo PRIVATE framework)

target_link_libraries(demo PRIVATE
    OgreMain
    OgreBites
    Codec_STBI
    OgrePaging
    imgui::imgui   
    fmt::fmt
)
#

target_include_directories(demo PRIVATE include)

# 如果 demo 有额外依赖（如 imgui），在这里 find_package 并链接
# find_package(imgui CONFIG REQUIRED)
# target_link_libraries(demo PRIVATE imgui::imgui)