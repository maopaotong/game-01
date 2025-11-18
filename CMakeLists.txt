# game-01/CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
option(BUILD_FRAMEWORK "Build framework" ON)
option(BUILD_DEMO "Build framework" OFF)
option(BUILD_GAME "Build framework" ON)
option(BUILD_TOOLS "Build framework" OFF)

project(Game01 LANGUAGES CXX)

# 设置全局标准（可选）
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(MSVC)
    add_compile_options(/utf-8)
endif()

# 添加子项目（顺序很重要：先 framework，后 demo）
if(BUILD_FRAMEWORK)
    add_subdirectory(framework)
endif()
if(BUILD_DEMO)
    add_subdirectory(demo)
endif()
if(BUILD_GAME)
    add_subdirectory(game)
endif()

if(BUILD_TOOLS)
    add_subdirectory(tools)
endif()
