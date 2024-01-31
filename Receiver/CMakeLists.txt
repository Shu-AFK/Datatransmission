# Receiver CMake file
add_executable(ReceiverExec src/main.cpp)

# Link against the Winsock library
if(WIN32)
    target_link_libraries(ReceiverExec PRIVATE Ws2_32 Mswsock AdvApi32)
endif()