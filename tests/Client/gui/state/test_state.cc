#include "src/gui/state.h"
#include "src/gui/password.h"
#include "src/gui/structs.h"

#include "catch.hpp"

Connection createDummyConnection() {
    Connection conn;
    conn.client = std::make_shared<Client>("127.0.0.1", "27015", "abc", "def", false);
    strncpy(conn.buttonName, "HelloWorld!", sizeof(conn.buttonName) - 1);
    conn.buttonName[sizeof(conn.buttonName) - 1] = '\0';
    return conn;
}

TEST_CASE("Test isSingleByteChars", "[isSingleByteChars]") {
    REQUIRE(isSingleByteChars("hello") == true);
    REQUIRE(isSingleByteChars("héllo") == false);
    REQUIRE(isSingleByteChars("12345") == true);
}

TEST_CASE("Test appendBinaryLength", "[appendBinaryLength]") {
    std::stringstream stream;
    appendBinaryLength(stream, 42);

    uint32_t output;
    stream.read(reinterpret_cast<char*>(&output), sizeof(output));

    REQUIRE(output == 42);
}

TEST_CASE("Test getState", "[getState]") {
    std::vector<Connection> connections = {
            createDummyConnection(),
            createDummyConnection(),
    };

    std::string ret;
    REQUIRE_NOTHROW(ret = getState(connections));
    REQUIRE(!ret.empty());
}

TEST_CASE("Test compressState and decompressState", "[compressState][decompressState]") {
    const std::string inputState = "This is a test state string";
    std::string compressedState = compressState(inputState);

    REQUIRE(!compressedState.empty());

    std::string decompressedState = decompressState(compressedState, inputState.size());

    REQUIRE(decompressedState == inputState);
}

TEST_CASE("Test saveState", "[saveState]") {
    std::vector<Connection> connections = {
            createDummyConnection(),
            createDummyConnection(),
    };

    std::string error;
    int result = saveState("sample_state", connections, error);

    REQUIRE(result == 0);
    REQUIRE(error.empty());
}