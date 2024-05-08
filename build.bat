@echo off
REM Make a new directory to store the build files
if not exist build (
    mkdir build
) else (
    echo "Build directory already exists"
)

REM Navigate into the build directory
cd build

REM The first command line argument is used as the vcpkg path; if none is provided, default is D:\vcpkg
if "%~1"=="" (
    set VCPKG_PATH=-DVCPKG_ROOT=D:\vcpkg
) else (
    set VCPKG_PATH=-DVCPKG_ROOT=%~1
)

REM Run the CMake configuration
cmake %VCPKG_PATH% ..

REM Build the project
cmake --build . --config Release

echo "Build completed"