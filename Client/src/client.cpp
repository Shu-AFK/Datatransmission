// TODO: Fix \t in front of "shell $" for GUI

#include <format>
#include "client.h"
#include <lz4.h>
#include <filesystem>

#define RECVBUFLENGTH 1000000

/**
 * @brief Returns the filename for the next log file.
 *
 * @details
 * This function searches for the highest numbered log file in the
 * current directory and returns the filename of the next log file to be created.
 *
 * @return The filename for the next log file.
 */
std::string Client::getLogFilename() {
    int highestCount = 0;

    for(const auto &entry : std::filesystem::directory_iterator(std::filesystem::current_path())) {
        std::string filename = entry.path().filename().string();
        if(filename.substr(0, 3) == "log" && filename.substr(filename.length() - 4, 4) == ".txt") {
            std::string numPart = filename.substr(3, filename.length() - 7);
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

    return std::format("log{}.txt", highestCount + 1);
}

/**
 * @brief Runs the client program.
 *
 * @details
 * This function is responsible for the main execution of the client program.
 * It takes user input, sends commands to the server, receives responses,
 * and handles file transfers. It also logs the commands and responses.
 * The function runs an infinite loop until the user explicitly closes
 * the connection or an error occurs.
 */
void Client::run() {
    while (true) {
        // Clears the strings
        std::string command;
        std::cout << "shell $ ";
        std::getline(std::cin, command);
        int res = sendCommand(command);

        if(res == 1)
            break;

        if(res < 0)
            continue;
    }

    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        throw std::runtime_error(std::format("shutdown failed with error: {}", WSAGetLastError()));
    }
    closeConnection();
}

/**
 * @brief Initialize the Winsock library.
 *
 * @details
 * This function initializes the Winsock library by calling WSAStartup.
 * It sets the version of Winsock to be used (2.2) and loads the required
 * DLLs. If the initialization fails, it throws a runtime_error with a
 * descriptive message.
 *
 * @throw std::runtime_error Initialization of Winsock library failed.
 */
void Client::initWinsock() {
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        throw std::runtime_error("WSAStartup failed");
    }
}

/**
 * @brief Initializes a connection to the server.
 *
 * @details
 * This function initializes a connection to the server specified by the server name and port number. It performs DNS
 * resolution to obtain the server's address information, creates and connects a socket to the server, and stores the
 * connected socket in the `ConnectSocket` member variable.
 *
 * @param server_name The name or IP address of the server to connect to.
 *
 * @throws std::runtime_error if getaddrinfo fails to obtain the server's address information.
 */
void Client::initServerConnection(const char *server_name) {
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    iResult = getaddrinfo(server_name, port.c_str(), &hints, &result);
    if (iResult != 0) {
        throw std::runtime_error("getaddrinfo failed");
    }

    ConnectSocket = createAndConnectSocket();
    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server\n");
        WSACleanup();
        throw std::runtime_error("unable to connect to server");
    }

    // Authentication
    std::string iSendString = std::format("auth: {} {}", username, password);
    sendData(ConnectSocket, iSendString);

    std::string valid = recvData(ConnectSocket, None);
    if(valid != "valid")
        throw std::runtime_error("Credentials are invalid");
}

/**
 * @brief Creates and connects a socket to a server.
 *
 * @details
 * This function iterates through the list of address information
 * structs and attempts to create and connect a socket for each
 * address until a successful connection is made. It returns the
 * connected socket or INVALID_SOCKET if no successful connection
 * was made.
 *
 * @return The connected socket or INVALID_SOCKET.
 */
SOCKET Client::createAndConnectSocket() {
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %d\n", WSAGetLastError());
            WSACleanup();
            exit(1);
        }

        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int) ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        return ConnectSocket; // return the socket if connection is successful
    }
    return INVALID_SOCKET;  // return invalid socket if no connection was successful
}

/**
 * @brief Shifts the provided string to the left by the specified number of characters.
 *
 * @details
 * This function modifies the given string by removing the specified number of characters
 * from the beginning (left side) of the string. The modified string will be stored back in
 * the same variable.
 *
 * @param str The string to shift.
 * @param num The number of characters to remove from the beginning of the string.
 * @return Returns 0 if the operation is successful, otherwise returns 1.
 *
 * @note If the length of the string is larger than zero (empty string is not allowed)
 *       and the specified number is less than the length of the string, the operation is
 *       successful and the modified string is stored back in the same variable. Otherwise,
 *       the operation fails and returns an error code.
 */
int Client::shiftStrLeft(std::string &str, int num) {
    size_t len = str.length();

    if(len > 0 && num < len) {
        str = str.substr(num);
        return 0;
    }
    else
        return 1;
}

/**
 * @brief Sends data to the server.
 *
 * @details
 * This function sends the specified command to the server using the given client socket.
 *
 * @param clientSocket The socket to send the data through.
 * @param cmd The command to send.
 * @return 0 if the data is successfully sent, -1 otherwise.
 */
int Client::sendData(SOCKET clientSocket, std::string cmd)
{
    log << cmd << std::endl;
    cmd += '\f';

    int iSendResult = send(clientSocket, cmd.c_str(), (int) cmd.length(), 0);
    if(iSendResult == SOCKET_ERROR)
    {
        std::cout << "Error in sending command to server. Error: " << WSAGetLastError() << std::endl;
        log << "Error in sending command to server. Error: " << WSAGetLastError() << std::endl;
        return -1;
    }

    return 0;
}

/**
  * @brief Receives data from a client socket and stores it in a file.
  *
  * @details
  * This function receives data from the specified client socket and stores it in a file,
  * specified by the provided command string. If the received data is compressed, it will
  * be decompressed before storing it in the file.
  *
  * @param clientSocket The client socket to receive data from.
  * @param cmd The command string specifying the file to store the data in.
  * @return Returns a string indicating the status of the operation.
  *
  * @note The function will continue to receive data until a '\f' character is received,
  *       indicating the end of the data. If the connection is closed before the '\f'
  *       character is received, the function will return "Connection closed". If an error
  *       occurs during the receiving process, an empty string will be returned.
  */
std::string Client::recvData(SOCKET clientSocket, std::string cmd) {
    std::string ret;
    std::string file_contents;
    char recvChar;
    bool isCompressed = false;
    size_t originalSize = 0; // to store original size if it's compressed data
    size_t compressedSize = 0;
	char recvBuf[RECVBUFLENGTH];

    while(true) {
        int bytes_recvd = recv(clientSocket, &recvChar, 1, 0);

        if(bytes_recvd > 0) {
            if(recvChar == '\f')
                return ret;

            if(recvChar == '\v') {
                bytes_recvd = recv(clientSocket, &recvChar, 1, 0);
                if(bytes_recvd > 0) {
                    if(recvChar == '\v') {
                        if(isCompressed) {
                            // Read the sizes (original then compressed sizes)
                            recv(clientSocket, reinterpret_cast<char *>(&originalSize), sizeof(originalSize), 0);
                            recv(clientSocket, reinterpret_cast<char *>(&compressedSize), sizeof(compressedSize), 0);

                            // Receive the compressed data
                            char *compressedData = new char[compressedSize];
                            recv(clientSocket, compressedData, compressedSize, 0);

                            // Allocate a buffer for decompressed data
                            char *decompressedData = new char[originalSize];

                            // Decompress the data
                            int decompressedSize = LZ4_decompress_safe(compressedData, decompressedData, compressedSize, originalSize);

                            if(decompressedSize < 0) {
                                delete[] compressedData;
                                delete[] decompressedData;
                                return "An error occurred during decompression."; // in case of decompression error
                            }

                            // Assign the decompressed data back to file_contents
                            file_contents = std::string(decompressedData, decompressedData + decompressedSize);

                            delete[] compressedData;
                            delete[] decompressedData;
                        }
						else {
							int nBytes = recv(clientSocket, recvBuf, RECVBUFLENGTH, 0); // receive if file less, then 1 Mb
							file_contents.assign(recvBuf, nBytes -1); // -1 for removing \f
						}
						
						std::string msg;
                        if (cmd.compare(0, 8, "copy_to ") == 0) {
							shiftStrLeft(cmd, 8);
							msg = "copied";
						}
						else if (cmd.compare(0, 4, "cut ") == 0) {
							shiftStrLeft(cmd, 4);
							msg = "cut";
						}

						std::ofstream output(cmd, std::ios::out | std::ios::binary);
						output << file_contents;
						output.close();
						std::string ret = std::format("File has been {} successfully!", msg);
						return ret;
                    }

                    else if(recvChar == '\r') {
                        isCompressed = true;
                    }
                }
            }

            ret += recvChar;
        }

        else if (bytes_recvd == 0) {
            return "Connection closed";
        }

        else {
            return "";
        }

    }
}

void Client::closeConnection() {
    std::cout << "Closing connection..." << std::endl;
    log.close();
    closesocket(ConnectSocket);
    WSACleanup();
}

/*
 *  1 = exit
 * -1 = continue
 * -2 = error but continue
 */
int Client::sendCommand(std::string command) {
#ifdef DATATRANSMISSION_CLIENT_GUI
    content += std::format("shell ${}\n\t", command);
#endif
    bool isCopyFrom = false;

    if(command.empty())
        return -1;

    log << "shell $ " << command << std::endl;

    if (strncmp(command.c_str(), "exit", 4) == 0) {
        return 1;
    }

    // Checks if the typed command is copy_from, due to it needing different procedure
    if(strncmp(command.c_str(), "copy_from ", 10) == 0)
        isCopyFrom = true;

    // send command to server
    int iSendResult = sendData(ConnectSocket, command);
    if(iSendResult == -1) {
        std::string errorMessage = "Failed to send data, error: " + std::to_string(WSAGetLastError());
        log << errorMessage << std::endl;
#ifndef DATATRANSMISSION_CLIENT_GUI
        std::cerr << errorMessage << std::endl;
#else
        content += std::format("{}\n", errorMessage);
#endif
        return -2;
    }

    if(isCopyFrom) {
        shiftStrLeft(command, 10);

        std::ifstream input(command);
        if(!input) {
            std::string errorMessage = "Failed to open file";
#ifndef DATATRANSMISSION_CLIENT_GUI
            std::cerr << errorMessage << std::endl;
#else
            content += std::format("{}\n", errorMessage);
#endif
            log << errorMessage << std::endl;
            return -2;
        }

        // Sends the file send sequence
        std::string file_contents;
        char c;
        while(input.get(c))
            file_contents += c;

        input.close();

        // Checks if the file is bigger than 1MB, if yes it's getting compressed before getting sent
        std::filesystem::path file{command};
        size_t originalSize = file_contents.size();

        if(std::filesystem::file_size(file) > 1000000) {
            int maxCompressedSize = LZ4_compressBound(static_cast<int>(originalSize));
            char *compressed = new char[maxCompressedSize];
            int compressedSize = LZ4_compress_default(file_contents.c_str(), compressed, static_cast<int>(file_contents.size()), maxCompressedSize);

            if(compressedSize < 0) {
                free(compressed);
#ifndef DATATRANSMISSION_CLIENT_GUI
                std::cerr << "Error in compressing file" << std::endl;
#else
                content += "Error in compressing file\n";
#endif
                log << "Error in compressing file" << std::endl;
                return -2;
            }

            file_contents.clear();
            file_contents += "\r";
            file_contents += std::string(reinterpret_cast<char*>(&originalSize), sizeof(originalSize)); // Add original size
            file_contents += std::string(reinterpret_cast<char*>(&compressedSize), sizeof(compressedSize)); // Add compressed size
            file_contents.append(compressed, compressed + compressedSize); // Add compressed data

            free(compressed);
        }

        file_contents.insert(0, "\v\v");
        file_contents += '\f';

        iSendResult = send(ConnectSocket, file_contents.c_str(), (int) file_contents.length(), 0);
        if(iSendResult == SOCKET_ERROR) {
            std::string errorMessage = std::format("Failed to send file contents, error: {}", std::to_string(WSAGetLastError()));
#ifndef DATATRANSMISSION_CLIENT_GUI
            std::cerr << errorMessage << std::endl;
#else
            content += std::format("{}\n", errorMessage);
#endif
            log << errorMessage << std::endl;
            return -2;
        }
    }

    // read response from server
    std::string response = recvData(ConnectSocket, command);
    if (response.empty()) {
        std::string errorMessage = "Failed to receive data, error: " + std::to_string(WSAGetLastError());
        log << errorMessage << std::endl;
#ifndef DATATRANSMISSION_CLIENT_GUI
        std::cerr << errorMessage << std::endl;
#else
        content += std::format("{}\n", errorMessage);
#endif
        return -2;
    }

    else if (response == "Connection closed") {
#ifndef DATATRANSMISSION_CLIENT_GUI
        std::cout << "Connection closing..." << std::endl;
#else
        content += "Connection closing...\n";
#endif
        log << "Connection closing..." << std::endl;
        return 1;
    }

    else {
#ifndef DATATRANSMISSION_CLIENT_GUI
        std::cout << response << std::endl;
#else
        content += std::format("{}\n", insertTabAfterNewline(response));
#endif
        log << response << std::endl;
    }

    return 0;
}

#ifdef DATATRANSMISSION_CLIENT_GUI
std::string Client::getBuffer() {
    return content;
}

std::string Client::getIP() {
    return ip;
}

std::string Client::insertTabAfterNewline(const std::string &input) {
    std::string result = input;
    std::string::size_type pos = 0;
    while ((pos = result.find('\n', pos)) != std::string::npos) {
        result.insert(pos + 1, "\t");
        pos += 2;
    }
    return result;
}
#endif
