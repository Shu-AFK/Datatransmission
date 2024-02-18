include_directories(${SQLITE_PATH})

# Server CMake file
add_executable(Server src/main.cpp
        src/helper.h
        src/helper.cpp
        src/server.h
        src/server.cpp)

# Link against the filesystem library if necessary
if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9)
    target_link_libraries(Server stdc++fs)
endif()

# Link against the Winsock library on Windows
if(WIN32)
    target_link_libraries(Server Ws2_32)
endif()

set(SQLITE_LIBRARIES "${SQLITE_PATH}\\sqlite3.dll")
target_link_libraries(Server ${SQLITE_LIBRARIES})