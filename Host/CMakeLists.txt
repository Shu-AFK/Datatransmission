include_directories("D:\\SQLite")

# Host CMake file
add_executable(HostExec src/main.cpp
        src/helper.h
        src/helper.cpp
        src/server.h
        src/server.cpp)

# Link against the Winsock library
if(WIN32)
    target_link_libraries(HostExec PRIVATE Ws2_32)
endif()

# Locate the sqlite3 library
set(SQLITE3_LIBRARY "D:\\SQLite\\sqlite3.dll")

# Make sure the library was found
if(NOT SQLITE3_LIBRARY)
    message(FATAL_ERROR "SQLite3 not found")
endif()

# Link against the filesystem library if necessary
if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9)
    target_link_libraries(HostExec stdc++fs)
endif()

# Corrected from MyProject to HostExec
target_link_libraries(HostExec PRIVATE ${SQLITE3_LIBRARY})