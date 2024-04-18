@echo off
REM Make a new directory to store the build files
if not exist build (
    mkdir build
) else (
    echo "Build directory already exists"
)

REM Navigate into the build directory
cd build

REM The first command line argument is used as the SQLite path; if none is provided, leave it blank
if "%~1"=="" (
    set SQLITE_PATH=
) else (
    set SQLITE_PATH=-DSQLITE_PATH=%~1
)

REM The second command line argument is used as the vcpkg path; if none is provided, default is D:\vcpkg
if "%~2"=="" (
    set VCPKG_PATH=-DVCPKG_ROOT=D:\vcpkg
) else (
    set VCPKG_PATH=-DVCPKG_ROOT=%~2
)

REM Run the CMake configuration
cmake %SQLITE_PATH% %VCPKG_PATH% ..

REM Build the project
cmake --build . --config Release

echo "Build completed"