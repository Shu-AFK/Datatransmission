# Client CMake file
add_executable(Client src/main.cpp
        src/client.h
        src/client.cpp
)

# Link against the Winsock library
if(WIN32)
    target_link_libraries(Client PRIVATE Ws2_32 Mswsock AdvApi32)
endif()

# Find LZ4 and link
include_directories(${INCLUDE_DIR})
target_link_libraries(Client PRIVATE ${LZ4_LIBRARY})