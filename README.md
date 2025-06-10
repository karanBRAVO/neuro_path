# NeuroPath

A maze game where you navigate through a grid to find the exit while avoiding obstacles.
This sits in an interesting niche between educational games and cognitive assessment tools. It's similar to spatial memory tests used in psychology but gamified. Think of it as a blend between classic maze games and brain training apps.

## Technologies Used

- C++ (cpp)
- [raylib](https://www.raylib.com/)
- [CMake](https://cmake.org/)
- [vcpkg](https://github.com/microsoft/vcpkg)

## Getting Started

## Prerequisites

1. Install [vcpkg](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started-vscode?pivots=shell-powershell)
2. Set `VCPKG_ROOT` environment variable
3. Install raylib:

```bash
vcpkg install raylib:x64-windows
```

## Build

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

## Run

```bash
# cd to the build/Debug directory
./NeuroPath.exe
```
