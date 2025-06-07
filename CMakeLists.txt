cmake_minimum_required(VERSION 3.15)

project(NeuroPath)

set(CMAKE_CXX_STANDARD 17)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "")
    message(STATUS "Using vcpkg toolchain: ${CMAKE_TOOLCHAIN_FILE}")
endif()

if(NOT DEFINED VCPKG_TARGET_TRIPLET)
    if(WIN32)
        set(VCPKG_TARGET_TRIPLET "x64-windows")
    elseif(UNIX AND NOT APPLE)
        set(VCPKG_TARGET_TRIPLET "x64-linux")
    elseif(APPLE)
        set(VCPKG_TARGET_TRIPLET "x64-osx")
    endif()
endif()

if(DEFINED ENV{VCPKG_ROOT})
    list(APPEND CMAKE_PREFIX_PATH "$ENV{VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}")
endif()

find_package(raylib CONFIG REQUIRED)

add_executable(NeuroPath main.cpp)

target_link_libraries(NeuroPath PRIVATE raylib)
