cmake_minimum_required(VERSION 3.12)
project(Datatransmission CXX)

# Set the default SQLite path as D:\SQLite
if(NOT DEFINED SQLITE_PATH)
    set(SQLITE_PATH "D:\\SQLite")
endif()

message(STATUS "Using SQLITE_PATH=${SQLITE_PATH}")

# Set CPP standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include subdirectories
add_subdirectory(Host)
add_subdirectory(Receiver)