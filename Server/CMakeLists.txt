include_directories(${SQLITE_PATH})

# Server CMake file
add_executable(Server src/main.cpp
        src/helper.h
        src/helper.cpp
        src/server.h
        src/server.cpp)

# Link against the filesystem library if necessary
if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9)
    target_link_libraries(Server PRIVATE stdc++fs)
endif()

# Link against the Winsock library on Windows
if(WIN32)
    target_link_libraries(Server PRIVATE Ws2_32)
    target_link_libraries(Server PRIVATE fwpuclnt) # fwpuclnt.lib for Winsock secure socket extensions
    target_link_libraries(Server PRIVATE ntdsapi) # ntdsapi.lib for DsMakeSpn function
endif()

# Include sqlite3
include_directories(Server PRIVATE ${SQLITE_INCLUDE_DIR})
target_link_libraries(Server PRIVATE ${SQLITE_LIBRARY})

# Include lz4
include_directories(${INCLUDE_DIR})
target_link_libraries(Server PRIVATE ${LZ4_LIBRARY})

# Include Libsodium
include_directories(${LIBSODIUM_INCLUDE_DIR})
target_link_libraries(Server PRIVATE ${LIBSODIUM_LIBRARY})