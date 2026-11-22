cmake_minimum_required(VERSION 3.15)

project(AutoXD VERSION 1.0 DESCRIPTION "Linux autoclicking tool" LANGUAGES CXX)

# Cpp >= 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBPORTAL REQUIRED IMPORTED_TARGET libportal)

# similar to importing packages
add_executable(autoxd
    src/main.cpp
    src/system/wayland.cpp
)

target_compile_features(autoxd PRIVATE cxx_std_17)
target_link_libraries(autoxd PRIVATE PkgConfig::LIBPORTAL)