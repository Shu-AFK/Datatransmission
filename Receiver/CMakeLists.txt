# Receiver CMake file
add_executable(ReceiverExec src/main.cpp)

# Link against the Winsock library
if(WIN32)
    target_link_libraries(ReceiverExec Ws2_32)
    target_link_libraries(ReceiverExec Mswsock)
    target_link_libraries(ReceiverExec AdvApi32)
endif()