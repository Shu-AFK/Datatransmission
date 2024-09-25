file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/tests/*.cc"
)

set(SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/Client/src/gui/state.cpp
        ${CMAKE_SOURCE_DIR}/Client/src/gui/password.cpp
        ${CMAKE_SOURCE_DIR}/Client/src/gui/structs.cpp
        ${CMAKE_SOURCE_DIR}/Client/src/client.cpp
)

include_directories(
        ${LIBSODIUM_INCLUDE_DIR}
        ${INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/catch2
        ${CMAKE_SOURCE_DIR}/Client
        ${CMAKE_SOURCE_DIR}/Server
        ${CMAKE_SOURCE_DIR}/Client/src/gui
)

add_executable(DatatransmissionTests ${TEST_SOURCES} ${SOURCE_FILES})

target_link_libraries(DatatransmissionTests PUBLIC
        ${LIBSODIUM_LIBRARY}
        ${LZ4_LIBRARY}
)

enable_testing()

add_test(NAME DatatransmissionTests COMMAND DatatransmissionTests)