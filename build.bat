@echo off
REM Make a new directory to store the build files
if not exist build (
    mkdir build
) else (
    echo "Build directory already exists"
)

REM Navigate into the build directory
cd build

REM Run the CMake configuration
cmake ..

REM Build the project
cmake --build . --config Release

echo "Build completed"