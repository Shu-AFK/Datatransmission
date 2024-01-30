# Host CMake file
add_executable(HostExec src/main.cpp
        src/helper.cpp)

# Link against the Winsock library
if(WIN32)
    target_link_libraries(HostExec Ws2_32)
endif()

# Link against the filesystem library if necessary
if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9)
    target_link_libraries(MyExecutable stdc++fs)
endif()