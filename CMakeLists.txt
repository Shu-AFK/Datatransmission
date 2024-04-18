if(POLICY CMP0079)
    cmake_policy(SET CMP0079 NEW)
endif()

cmake_minimum_required(VERSION 3.12)
project(Datatransmission CXX)

# vcpkg integration
set(VCPKG_ROOT "D:\\vcpkg" CACHE PATH "Path to vcpkg")
set(CMAKE_TOOLCHAIN_FILE ${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake CACHE STRING "")

set(LZ4_INCLUDE_DIR "${VCPKG_ROOT}/installed/x64-windows/include")
set(LZ4_LIBRARY "${VCPKG_ROOT}/installed/x64-windows/lib/lz4.lib")

# Set the default SQLite path as D:\SQLite
if(NOT DEFINED SQLITE_PATH)
    set(SQLITE_PATH "D:\\SQLite")
endif()

message(STATUS "Using SQLITE_PATH=${SQLITE_PATH}")

# Set CPP standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include subdirectories
add_subdirectory(Server)
add_subdirectory(Client)
add_subdirectory(tests)