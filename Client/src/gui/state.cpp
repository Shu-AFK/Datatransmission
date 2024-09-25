#include "state.h"

#include "password.h"
#include "config.h"

#include <filesystem>
#include <format>
#include <string>
#include <stdexcept>

#include <lz4.h>

std::string getStateFilename(const std::string &name) {
    int highestCount = 0;

    for (const auto &entry : std::filesystem::directory_iterator(std::filesystem::current_path())) {
        std::string filename = entry.path().filename().string();
        if (filename.length() > name.length() && filename.substr(0, name.length()) == name && filename.substr(filename.length() - 6, 6) == ".state") {
            std::string numPart = filename.substr(name.length(), filename.length() - 6 - name.length());
            try {
                int num = std::stoi(numPart);
                if (num > highestCount) {
                    highestCount = num;
                }
            } catch (std::invalid_argument&) {
                continue;
            }
        }
    }

    return std::format("{}{}.state", name, highestCount == 0 ? "" : std::to_string(highestCount + 1));
}

bool isSingleByteChars(const std::string& str) {
    for (unsigned char c : str) {
        if (c > 127) {
            return false;
        }
    }
    return true;
}

void appendBinaryLength(std::stringstream& stream, uint32_t length) {
    stream.write(reinterpret_cast<const char*>(&length), sizeof(length));
}

std::string getState(const std::vector<Connection>& connections) {
    std::stringstream state(std::stringstream::binary | std::stringstream::out);

    for (const auto& conn : connections) {
        if (!isSingleByteChars(conn.client->username) && !isSingleByteChars(conn.client->password)) {
            throw std::runtime_error("Username and/or password contains non-ascii characters");
        }

        uint32_t ipLength = static_cast<uint32_t>(conn.client->ip.length());
        uint32_t portLength = static_cast<uint32_t>(conn.client->port.length());
        uint32_t usernameLength = static_cast<uint32_t>(conn.client->username.length());
        uint32_t buttonNameLength = static_cast<uint32_t>(strlen(conn.buttonName));

        appendBinaryLength(state, ipLength);
        state.write(conn.client->ip.c_str(), ipLength);

        appendBinaryLength(state, portLength);
        state.write(conn.client->port.c_str(), portLength);

        appendBinaryLength(state, usernameLength);
        state.write(conn.client->username.c_str(), usernameLength);

        std::string passwordEnc = encryptPassword(conn.client->password, static_cast<unsigned char *>(conn.key));
        uint32_t passwordLength = static_cast<uint32_t>(passwordEnc.length());

        appendBinaryLength(state, passwordLength);
        state.write(passwordEnc.c_str(), passwordLength);

        appendBinaryLength(state, buttonNameLength);
        state.write(conn.buttonName, buttonNameLength);
    }

    return state.str();
}

std::string compressState(const std::string &state) {
    int maxCompressedSize = LZ4_compressBound(state.size());
    std::string compressed(maxCompressedSize, '\0');

    int compressedSize = LZ4_compress_default(state.data(), compressed.data(), state.size(), maxCompressedSize);
    if (compressedSize <= 0) {
        throw std::runtime_error("Invalid compression size");
    }

    compressed.resize(compressedSize);
    return compressed;
}

std::string decompressState(const std::string& compressedData, int originalSize) {
    std::string decompressedData(originalSize, '\0');

    int decompressedSize = LZ4_decompress_safe(
            compressedData.data(),
            decompressedData.data(),
            compressedData.size(),
            originalSize
    );

    if (decompressedSize < 0) {
        throw std::runtime_error("Decompression failed");
    }

    decompressedData.resize(decompressedSize);

    return decompressedData;
}

int saveState(const std::string &filename, const std::vector<Connection> &connections, std::string &error) {
    error = "";

    try {
        std::string state = getState(connections);
        if (state.empty()) {
            throw std::runtime_error("Failed to get state info");
        }

        std::string stateFilename = getStateFilename(filename);
        std::ofstream file(stateFilename, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file");
        }

        std::string content;

        if (state.length() > sizeof(uint32_t) * 256) {
            content = compressState(state);
            if (content.empty()) {
                throw std::runtime_error("Failed to compress state");
            }
            file.write(reinterpret_cast<const char *>(STATE_COMPRESSED), 1);
            file.write(reinterpret_cast<char *>(static_cast<uint32_t>(state.length())), sizeof(uint32_t));
        } else {
            file.write(reinterpret_cast<const char *>(STATE_UNCOMPRESSED), 1); // TODO: ERROR
            content = state;
        }

        file.write(content.data(), content.size());
        if (file.fail()) {
            throw std::runtime_error("Failed to write compressed state to file");
        }

        file.close();
    } catch (const std::runtime_error &e) {
        std::cerr << "Error in save state: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}

Connection readConnection(std::stringstream& stateStream) {
    Connection conn;

    uint32_t ipLength, portLength, usernameLength, passwordLength, buttonNameLength;

    if (!stateStream.read(reinterpret_cast<char*>(&ipLength), sizeof(ipLength))) throw std::runtime_error("Failed to read IP length");
    std::string ip(ipLength, '\0');
    if (!stateStream.read(&ip[0], ipLength)) throw std::runtime_error("Failed to read IP");

    if (!stateStream.read(reinterpret_cast<char*>(&portLength), sizeof(portLength))) throw std::runtime_error("Failed to read port length");
    std::string port(portLength, '\0');
    if (!stateStream.read(&port[0], portLength)) throw std::runtime_error("Failed to read port");

    if (!stateStream.read(reinterpret_cast<char*>(&usernameLength), sizeof(usernameLength))) throw std::runtime_error("Failed to read username length");
    std::string username(usernameLength, '\0');
    if (!stateStream.read(&username[0], usernameLength)) throw std::runtime_error("Failed to read username");

    if (!stateStream.read(reinterpret_cast<char*>(&passwordLength), sizeof(passwordLength))) throw std::runtime_error("Failed to read password length");
    std::string encryptedPassword(passwordLength, '\0');
    if (!stateStream.read(&encryptedPassword[0], passwordLength)) throw std::runtime_error("Failed to read encrypted password");
    std::string password = decryptPassword(encryptedPassword, conn.key);

    conn.client = std::make_shared<Client>(ip, port, username, password);

    if (!stateStream.read(reinterpret_cast<char*>(&buttonNameLength), sizeof(buttonNameLength))) throw std::runtime_error("Failed to read button name length");
    if (!stateStream.read(conn.buttonName, buttonNameLength)) throw std::runtime_error("Failed to read button name");
    conn.buttonName[buttonNameLength] = '\0';

    return conn;
}

int loadState(const std::string &path, std::vector<Connection> &connections, std::string &error) {
    error = "";

    try {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + path);
        }

        char stateType;
        file.read(&stateType, 1);

        if (file.fail()) {
            throw std::runtime_error("Failed to read state type from file");
        }

        std::string content;
        if (stateType == STATE_COMPRESSED) {
            uint32_t uncompressedSize;
            file.read(reinterpret_cast<char*>(&uncompressedSize), sizeof(uncompressedSize));

            if (file.fail()) {
                throw std::runtime_error("Failed to read uncompressed size from file");
            }

            std::vector<char> compressedData((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            content = decompressState(std::string(compressedData.begin(), compressedData.end()), uncompressedSize);
        } else if (stateType == STATE_UNCOMPRESSED) {
            content = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        } else {
            throw std::runtime_error("Invalid state type in file");
        }

        file.close();

        // Deserialize the state content back to connections
        std::stringstream stateStream(content, std::stringstream::binary | std::stringstream::in);
        while (stateStream.peek() != std::stringstream::traits_type::eof()) {
            connections.push_back(readConnection(stateStream));
        }
    } catch (const std::runtime_error &e) {
        std::cerr << "Error in loadState: " << e.what() << std::endl;
        error = std::format("Error in loadState: {}", e.what());
        return -1;
    }

    return 0;
}