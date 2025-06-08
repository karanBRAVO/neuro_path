#!/bin/bash
echo "Building the project..."

if [ "$(basename "$PWD")" != "build" ]; then
    echo "Please run this script from the 'build' directory."
    exit 1
fi

cmake ..
if [ $? -ne 0 ]; then
    echo "CMake configuration failed. Exiting."
    exit 1
fi
echo "CMake configuration completed successfully."

cmake --build .
if [ $? -ne 0 ]; then
    echo "Build failed. Exiting."
    exit 1
fi
echo "Build completed successfully."
