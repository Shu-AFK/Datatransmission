if(POLICY CMP0079)
    cmake_policy(SET CMP0079 NEW)
endif()

cmake_minimum_required(VERSION 3.12)
project(Datatransmission CXX)

# vcpkg integration
set(VCPKG_ROOT "D:\\vcpkg" CACHE PATH "Path to vcpkg")

set(INCLUDE_DIR "${VCPKG_ROOT}/installed/x64-windows/include")

set(LZ4_LIBRARY "${VCPKG_ROOT}/installed/x64-windows/lib/lz4.lib")

set(LIBSODIUM_INCLUDE_DIR "${VCPKG_ROOT}/installed/x64-windows/include/sodium")
set(LIBSODIUM_LIBRARY "${VCPKG_ROOT}/installed/x64-windows/lib/libsodium.lib")

set(SQLITE_INCLUDE_DIR "${VCPKG_ROOT}/installed/x64-windows/include/sqlite3")
set(SQLITE_LIBRARY "${VCPKG_ROOT}/installed/x64-windows/lib/sqlite3.lib")

# Set CPP standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include subdirectories
add_subdirectory(Server)
add_subdirectory(Client)
add_subdirectory(Client/src/gui)
add_subdirectory(tests)