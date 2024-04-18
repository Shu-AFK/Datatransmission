# Add the main.cc file
add_executable(DatatransmissionTests main.cc)

# Include the directory with catch.hpp
target_include_directories(DatatransmissionTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/catch2)

# The add_test command can replace catch_discover_tests
add_test(NAME DatatransmissionTests COMMAND DatatransmissionTests)