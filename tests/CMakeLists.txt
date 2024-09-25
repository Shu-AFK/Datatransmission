file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/tests/*.cc"
)

list(REMOVE_ITEM TEST_SOURCES "${CMAKE_SOURCE_DIR}/tests/test_main.cc")

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

add_subdirectory(catch2)

add_executable(DatatransmissionTests ${TEST_SOURCES} ${SOURCE_FILES})

if(WIN32)
    target_link_libraries(DatatransmissionTests PRIVATE Ws2_32 Mswsock AdvApi32)
endif()

target_link_libraries(DatatransmissionTests PRIVATE
        ${LIBSODIUM_LIBRARY}
        ${LZ4_LIBRARY}
        Catch2::Catch2
)

enable_testing()

add_test(NAME DatatransmissionTests COMMAND DatatransmissionTests)