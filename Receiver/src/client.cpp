#include "client.h"

/**
 * @brief Runs the client application.
 *
 * @details
 * This function continuously prompts the user for a command, sends it to the server, and receives the response.
 * The received response is printed to the console and logged in a file.
 * If the response is "Connection closed", the function terminates the connection with the server and halts the loop.
 *
 * @note This function assumes that the necessary setup and connection to the server has already been performed.
 */
void Client::run() {
    while (true) {
        // Clears the strings
        std::string command;
        std::cout << "shell $ ";
        std::getline(std::cin, command);
        log << "shell $ " << command << std::endl;

        // send command to server
        int iSendResult = sendData(ConnectSocket, command);
        if(iSendResult == -1) {
            std::string errorMessage = "Failed to send data, error: " + std::to_string(WSAGetLastError());
            log << errorMessage << std::endl;
            throw std::runtime_error(errorMessage);
        }

        // read response from server
        std::string response = recvData(ConnectSocket, command);
        if (response.empty()) {
            std::string errorMessage = "Failed to receive data, error: " + std::to_string(WSAGetLastError());
            log << errorMessage << std::endl;
            throw std::runtime_error(errorMessage);
        }

        else if (response == "Connection closed") {
            std::cout << "Connection closing..." << std::endl;
            log << "Connection closing..." << std::endl;
            break;
        }

        else {
            std::cout << response << std::endl;
            log << response << std::endl;
        }
    }

    // shut down the connection
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
    }
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
        printf("Unable to connect to server!\n");
        WSACleanup();
        exit(1);
    }
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
            printf("socket failed with error: %ld\n", WSAGetLastError());
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
int Client::sendData(SOCKET clientSocket, const std::string& cmd)
{
    int iSendResult = send(clientSocket, cmd.c_str(), (int) cmd.length(), 0);
    if(iSendResult == SOCKET_ERROR)
    {
        std::cout << "Error in sending command to server. Error: " << WSAGetLastError() << std::endl;
        return -1;
    }

    return 0;
}

/**
 * @brief Receives data from the client socket.
 *
 * @details
 * This function receives data from the client socket character by character until a special
 * character '\f' is encountered. It stores the received data in a string 'ret'. If the received
 * character is '\v', it indicates that a file is being sent. The function receives the file data
 * and writes it to a file with the name specified in the 'cmd' parameter. The function returns
 * appropriate messages based on the success or failure of the operation.
 *
 * @param clientSocket The socket to receive data from.
 * @param cmd The name of the file to write if a file is received.
 * @return Returns a string message indicating the success or failure of the operation.
 *
 * @note This function assumes that the client socket is already connected and active.
 *       Upon successful file transfer, the function returns "File has been copied successfully!".
 *       If the connection is closed before completing the transfer, it returns "Connection closed".
 *       If any error occurs during the transfer, it returns an empty string.
 */
std::string Client::recvData(SOCKET clientSocket, std::string& cmd) {
    std::string ret;
    std::string file_contents;
    char recvChar;

    while(true) {
        int bytes_recvd = recv(clientSocket, &recvChar, 1, 0);

        if(bytes_recvd > 0) {
            if(recvChar == '\f')
                return ret;

            if(recvChar == '\v') {
                bytes_recvd = recv(clientSocket, &recvChar, 1, 0);
                if(bytes_recvd > 0) {
                    if(recvChar == '\v') {
                        while(recvChar != '\f')
                        {
                            bytes_recvd = recv(clientSocket, &recvChar, 1, 0);

                            if(bytes_recvd > 0)
                                file_contents += recvChar;

                            else if (bytes_recvd == 0) // connection closed
                                return "Connection closed";

                            else // error
                                return "";
                        }

                        shiftStrLeft(cmd, 8);
                        std::ofstream output(cmd);
                        output << file_contents;
                        output.close();
                        return "File has been copied successfully!";
                    }
                }
            }

            ret += recvChar;
        }

        else if (bytes_recvd == 0)
            return "Connection closed";
        else
            return "";
    }
}