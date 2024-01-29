# Host CMake file
add_executable(HostExec src/main.cpp
        src/helper.cpp)

# Link against the Winsock library
if(WIN32)
    target_link_libraries(HostExec Ws2_32)
endif()