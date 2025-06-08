cmake_minimum_required(VERSION 3.15)

project(NeuroPath)

set(CMAKE_CXX_STANDARD 17)

message(STATUS "Root directory: ${CMAKE_CURRENT_SOURCE_DIR}")

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
message(STATUS "Using raylib version: ${raylib_VERSION}")

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
file(GLOB_RECURSE SOURCES
    "${SRC_DIR}/main.cpp"
    "${SRC_DIR}/maze-generator/*.cpp"
)

add_executable(NeuroPath ${SOURCES})

target_include_directories(NeuroPath PRIVATE
    "${SRC_DIR}"
    "${SRC_DIR}/maze-generator"
    "${SRC_DIR}/utils"
)

target_link_libraries(NeuroPath PRIVATE raylib)
