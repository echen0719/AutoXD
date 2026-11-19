cmake_minimum_required(VERSION 3.15)
project(AutoXD VERSION 1.0 DESCRIPTION "Linux autoclicking tool" LANGUAGES CXX)

# Cpp >= 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# similar to importing packages
add_library(utils STATIC
    utils/system/input.cpp
    utils/timing/timer.cpp
)

target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/utils)

add_executable(autoxd
    src/main.cpp
    src/autoclicker/autoclicker.cpp
)

target_include_directories(autoxd PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(autoxd PRIVATE utils)