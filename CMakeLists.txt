cmake_minimum_required(VERSION 3.27)
project(Datatransmission CXX)

# Set CPP standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include subdirectories
add_subdirectory(Host)
add_subdirectory(Receiver)
