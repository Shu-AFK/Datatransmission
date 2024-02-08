#include <chrono>
#include <cstdlib>
#include "server.h"

/**
 * @brief Handles the command received from the client.
 *
 * @details
 * This function receives a command from the client and performs the corresponding operation.
 * The function checks the command against a list of supported commands and calls the appropriate
 * handler function for each command. If the command is not recognized, an error message is sent
 * back to the client. The result of the command execution is returned as an integer value.
 *
 * @note It is assumed that the Server class has been properly initialized before calling this function.
 *
 * @param command The command received from the client.
 * @return An integer value representing the result of the command execution:
 *         - 0: The command was handled successfully.
 *         - 1: An error occurred while executing the command.
 *         - 2: The exit command was received.
 */
int Server::handleCommand(char* command) {
    if (strncmp(command, "pwd", 3) == 0) {
        if(handlePwdCommand() == -1) {
            handleError("pwd");
            return 1;
        }
        return 0;
    }
    else if(strncmp(command, "copy_from ", 10) == 0) {
        int res = handleCopyFromCommand(command);
        if(res == -1) {
            handleError("copy_from");
            return 1;
        }
        else if(res == -2) { // Time out return
            handleTimeout();
            return 1;
        }
        return 0;
    }
    else if (strcmp(command, "exit") == 0) {
        handleExitCommand();
        return 2;
    }
    else if (strncmp(command, "cd ", 3) == 0) {
        if(handleChangeDirectoryCommand(command + 3) == -1){
            handleError("cd");
            return 1;
        }
        return 0;
    }
    else if (strncmp(command, "ls", 2) == 0) {
        if(handleLsCommand(command) == -1){
            handleError("ls");
            return 1;
        }
        return 0;
    }
    else if (strncmp(command, "mkdir ", 6) == 0) {
        if(handleMakeDirectoryCommand(command) == -1){
            handleError("mkdir");
            return 1;
        }
        return 0;
    }
    else if (strncmp(command, "touch ", 6) == 0) {
        if(handleTouchFileCommand(command) == -1){
            handleError("touch");
            return 1;
        }
        return 0;
    }
    else if (strncmp(command, "rm ", 3) == 0) {
        if(handleRemoveFileCommand(command) == -1){
            handleError("rm");
            return 1;
        }
        return 0;
    }
    else if (strncmp(command, "rmdir ", 6) == 0) {
        if(handleRemoveDirectoryCommand(command) == -1){
            handleError("rmdir");
            return 1;
        }
        return 0;
    }
    else if(strncmp(command, "run ", 4) == 0) {
        if (handleRunCommand(command) == -1) {
            handleError("run");
            return 1;
        }
        return 0;
    }
    else if (strncmp(command, "copy_to ", 8) == 0) {
        if(handleCopyCommand(command) == -1) {
            handleError("copy_pc");
            return 1;
        }
        return 0;
    }
    else if (strncmp(command, "cat ", 4) == 0) {
        if(handleCatCommand(command) == -1) {
            handleError("cat");
            return 1;
        }
        return 0;
    }
    else if (strncmp(command, "echo ", 5) == 0) {
        if(handleEchoCommand(command) == -1) {
            handleError("echo");
            return 1;
        }
        return 0;
    }
    else if(strcmp(command, "move_startup") == 0) {
        int res = move_start();
        if(res == -1) {
            handleError("move_startup");
            return 1;
        }
        else if(res == -2) {
            handleStartupError(1);
            return 1;
        }
        return 0;
    }
    else if(strcmp(command, "remove_startup") == 0) {
        int res = remove_start();
        if(res == -1) {
            handleError("remove_startup");
            return 1;
        }
        else if(res == -2) {
            handleStartupError(2);
            return 1;
        }
        return 0;
    }
    else if(strncmp(command, "mv ", 3) == 0) {
        std::string first_arg;
        std::string second_arg;
        int space_counter = 0;

        for(int i = 0, len = (int) strlen(command); i < len; i++) {
            if(command[i] == ' ') {
                space_counter++;
                continue;
            }

            if(space_counter == 1)
                first_arg += command[i];
            if(space_counter == 2)
                second_arg += command[i];
        }

        if(space_counter != 2) {
            handleError("mv");
            return 1;
        }
        if(handleMoveCommand(command) == -1) {
            handleError("mv");
            return 1;
        }
        return 0;
    }
    else if(strncmp(command, "cp ", 3) == 0) {
        if(handleCpCommand(command) == -1) {
            handleError("cp");
            return 1;
        }
        return 0;
    }
    else if(strncmp(command, "find ", 5) == 0) {
        if(handleFindCommand(command) == -1) {
            handleError("find");
            return 1;
        }
        return 0;
    }
    else if(strncmp(command, "grep ", 5) == 0) {
        if (handleGrepCommand(command) == -1) {
            handleError("grep");
            return 1;
        }
        return 0;
    }
    else if(strcmp(command, "check_startup") == 0) {
        if (handleCheckInStartup() == -1) {
            handleError("check_startup");
            return 1;
        }
        return 0;
    }
    else {
        if(sendCmdDoesntExist()) {
            handleError("send");
            return 1;
        }
        return 0;
    }
}

/**
 * @brief Set up the server port by creating helper scripts and running initialization scripts.
 *
 * @details
 * This function creates helper scripts by calling the `create_scripts` function with the provided port.
 * If creating the scripts fails, it logs an error message and returns false.
 *
 * Then, it runs the initialization scripts by calling the `run_init` function.
 * If running the scripts fails, it logs an error message and returns false.
 *
 * @return true if the server port is successfully set up, false otherwise.
 */
bool Server::setupPort(){
    std::filesystem::path cwd = std::filesystem::current_path();

    // runs helper scripts
    if(run_init(port) != 0) {
        log << "Failed to run scripts" << std::endl;
        return false;
    }
    if(create_start_script(cwd.string(), port) != 0) {
        log << "Failed to create start script" << std::endl;
        return false;
    }
    return true;
}

/**
 * @brief Initializes the server.
 *
 * @details
 * This function initializes the server by performing the following steps:
 * 1. Calls WSAStartup to initialize the Winsock library.
 * 2. Checks for any errors during the initialization process and returns false if there is an error.
 * 3. Sets up the server address and port using getaddrinfo.
 * 4. Checks for any errors in resolving the address and returns false if there is an error.
 * 5. Creates a socket for the server to listen for client connections.
 * 6. Checks for any errors in creating the socket and returns false if there is an error.
 * 7. Binds the socket to the server address and port.
 * 8. Checks for any errors in binding the socket and returns false if there is an error.
 * 9. Frees the memory allocated for the address information.
 * 10. Listens for client connections on the socket.
 * 11. Checks for any errors in listening for client connections and returns false if there is an error.
 * 12. Accepts a client socket.
 * 13. Checks for any errors in accepting the client socket and returns false if there is an error.
 * 14. Closes the listening socket.
 * 15. Returns true to indicate successful initialization.
 *
 * @return bool - true if the server is successfully initialized, false otherwise.
 */
bool Server::initServer() {
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if(iResult != 0) {
        std::cerr << "WSAStartup failed with error: " << iResult << "\n";
        log << "WSAStartup failed with error: " << iResult << std::endl;
        return false;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, port.c_str(), &hints, &result);
    if(iResult != 0) {
        std::cerr << "getaddrinfo failed with error: " << iResult << "\n";
        log << "getaddrinfo failed with error: " << iResult << std::endl;
        return false;
    }

    // Create a SOCKET for the server to listen for client connections
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if(ListenSocket == INVALID_SOCKET) {
        std::cerr << "socket failed with error: " << WSAGetLastError() << "\n";
        log << "socket failed with error: " << WSAGetLastError() << std::endl;
        return false;
    }

    // Set up the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if(iResult == SOCKET_ERROR) {
        std::cerr << "bind failed with error: " << WSAGetLastError() << "\n";
        log << "bind failed with error: " << WSAGetLastError() << std::endl;
        closesocket(ListenSocket);
        return false;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if(iResult == SOCKET_ERROR) {
        std::cerr << "listen failed with error: " << WSAGetLastError() << "\n";
        log << "listen failed with error: " << WSAGetLastError() << std::endl;
        closesocket(ListenSocket);
        return false;
    }

    // Accept a client socket
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if(ClientSocket == INVALID_SOCKET) {
        std::cerr << "accept failed with error: " << WSAGetLastError() << "\n";
        log << "accept failed with error: " << WSAGetLastError() << std::endl;
        closesocket(ListenSocket);
        return false;
    }

    closesocket(ListenSocket);
    return true;
}

/**
 * @brief Handle the "pwd" command by sending the current directory to the client.
 *
 * @details
 * The current directory on the server is obtained using std::filesystem::current_path().
 * The current directory is then sent to the client through the socket connection.
 * If the send operation fails, an error message is printed and -1 is returned.
 *
 * @return 0 if the command is handled successfully, -1 otherwise.
 */
int Server::handlePwdCommand() {
    std::string cwd = std::filesystem::current_path().string();
    printf("Current directory on server: %s\n", cwd.c_str());
    cwd += '\f';

    int iSendResult = send(ClientSocket, cwd.c_str(), (int) cwd.length(), 0);
    if (iSendResult == SOCKET_ERROR) {
        fprintf(stderr, "send failed with error: %d\n", WSAGetLastError());
        return -1;
    }

    log << "SUCCESS!" << std::endl;
    return 0;
}

/**
 * @brief Handles the exit command and closes the connection.
 *
 * This function is called when the client issues an exit command. It prints
 * a message indicating that the connection is being closed.
 *
 * @note This function does not handle any return values or exceptions.
 */
void Server::handleExitCommand() {
    printf("Closing connection...\n");
}

/**
 * @brief Changes the working directory of the server.
 *
 * @details
 * This function changes the current working directory of the server to the specified path.
 * If the path is valid and the directory is successfully changed, a success message is sent
 * back to the client. If an error occurs while changing the directory, an error message is
 * sent back to the client.
 *
 * @param path The path of the directory to change to.
 * @return 0 if the directory was successfully changed, -1 otherwise.
 */
int Server::handleChangeDirectoryCommand(const char *path) {
    try {
        std::filesystem::current_path(path);
        std::string cwd = std::filesystem::current_path().string();

        char sendBuf[DEFAULT_BUFLEN];
        int n = snprintf(sendBuf, DEFAULT_BUFLEN, "Changed working directory to %s\f", cwd.c_str());

        if (n >= DEFAULT_BUFLEN) {
            fprintf(stderr, "sendBuf is too small for the message\n");
            return -1;
        }

        int iSendResult = send(ClientSocket, sendBuf, n, 0);
        if (iSendResult == SOCKET_ERROR) {
            fprintf(stderr, "send failed with error: %d\n", WSAGetLastError());
            return -1;
        }

        log << "SUCCESS!" << std::endl;
        return 0;
    }
    catch (const std::filesystem::filesystem_error& e) {
        char sendBuf[DEFAULT_BUFLEN];
        snprintf(sendBuf, DEFAULT_BUFLEN, "Error changing directory: %s\n", e.what());

        int iSendResult = send(ClientSocket, sendBuf, (int) strlen(sendBuf), 0);
        if(iSendResult == SOCKET_ERROR) {
            fprintf(stderr, "send failed with error: %d\n", WSAGetLastError());
            return -1;
        }

        log << "SUCCESS!" << std::endl;
        return 0;
    }
}

/**
 * @brief Handles the "ls" command.
 *
 * @details
 * This function handles the "ls" command, which lists the contents of a directory.
 * It sends the directory listing to the client socket.
 *
 * @param command The command string received from the client.
 *
 * @return 0 if the operation is successful, -1 if an error occurs.
 *
 * @note The function assumes that the command parameter is either "ls" or "ls <directory>".
 *       If the command is "ls", the function will list the contents of the current directory.
 *       If the command is "ls <directory>", the function will list the contents of the specified directory.
 *       The function uses the ClientSocket member variable to send data to the client.
 *       The function also uses the log member variable to log success or failure of the operation.
 *       The function relies on the shiftStrLeft function to remove the "ls " prefix from the command.
 *       The function uses the std::filesystem library to perform directory operations.
 */
int Server::handleLsCommand(char *command) {
    std::string prevDirectory;
    std::string cwd;
    bool thisDirectory = false;

    if(strcmp(command, "ls") == 0)
        cwd = std::filesystem::current_path().string();
    else {
        try{
            shiftStrLeft(command, 3);
            prevDirectory = std::filesystem::current_path().string();
            std::filesystem::current_path(command);
            cwd = std::filesystem::current_path().string();
            thisDirectory = true;
        } catch(const std::exception& e) {
            std::cerr << "Error in handleLS not current directory, error code: " << e.what() << std::endl;
            return -1;
        }
    }
    try {
        std::string directoryContents = "Directory listing for " + cwd + "\n";

        // directory_iterator is used to traverse all the entries of directory
        for (const auto & entry : std::filesystem::directory_iterator(cwd)) {
            directoryContents += entry.path().filename().string() + "\n";
        }

        directoryContents += '\f';
        int iSendResult = send(ClientSocket, directoryContents.c_str(), (int) directoryContents.length(), 0);

        if (iSendResult == SOCKET_ERROR) {
            fprintf(stderr, "send failed with error: %d\n", WSAGetLastError());
            return -1;
        }

        if(thisDirectory)
            std::filesystem::current_path(prevDirectory);

        log << "SUCCESS!" << std::endl;
        return 0;

    } catch (const std::exception& e) {
        char sendBuf[DEFAULT_BUFLEN];
        snprintf(sendBuf, DEFAULT_BUFLEN, "Error executing ls: %s\f", e.what());

        int iSendResult = send(ClientSocket, sendBuf, (int) strlen(sendBuf), 0);
        if(iSendResult == SOCKET_ERROR) {
            fprintf(stderr, "send failed with error: %d\n", WSAGetLastError());
            return -1;
        }

        log << "SUCCESS!" << std::endl;
        return 0;
    }
}

/**
 * @brief Sends an error message to the client indicating that the command doesn't exist.
 *
 * @details
 * This function constructs an error message "The command doesn't exist" and sends it to the client.
 * The error message is also printed to the standard error stream and logged to the log file.
 *
 * @return int - Returns 0 if the error message is successfully sent, -1 otherwise.
 */
int Server::sendCmdDoesntExist() {
    std::string sendBuf = "The command doesn't exist";
    std::cerr << sendBuf << std::endl;
    log << sendBuf << std::endl;

    sendBuf += '\f';

    int iSendResult = send(ClientSocket, sendBuf.c_str(), (int) sendBuf.length() + 1, 0);
    if (iSendResult == SOCKET_ERROR) {
        log << "FAILED" << std::endl;
        return -1;
    }
    return 0;
}

/**
 * @brief Handles the "Make Directory" command.
 *
 * @details
 * This method creates a directory with the given path if it doesn't already exist.
 * It then sends a success message to the client and logs the success.
 *
 * @param path The path of the directory to be created.
 *
 * @returns 0 if the directory is successfully created, -1 otherwise.
 *
 * @note The path parameter should be a valid null-terminated C string.
 *       The function assumes that the initial 6 characters of the path are to be ignored.
 *       If the CreateDirectory function call fails, the function returns -1.
 *       Otherwise, it sends a success message to the client using the ClientSocket
 *       and logs the success message to the log file.
 */
int Server::handleMakeDirectoryCommand(char *path) {
    shiftStrLeft(path, 6);

    // Creates a directory if it doesn't exist already
    if(CreateDirectory(path, NULL) || ERROR_ALREADY_EXISTS == GetLastError())
    {
        std::string sendSuc = std::format("Directory {} was successfully created!\f", path);
        int iSendResult = send(ClientSocket, sendSuc.c_str(), (int) sendSuc.length(), 0);
        if(iSendResult == SOCKET_ERROR) {
            fprintf(stderr, "send failed with error: %d\n", WSAGetLastError());
            return -1;
        }

        log << "SUCCESS!" << std::endl;
        return 0;
    }
    else
        return -1;
}

/**
 * @brief Handles the touch file command received from the client.
 *
 * @details
 * This function handles the touch file command received from the client. It creates a new file with the specified
 * file name. If the file creation is successful, it sends a success message to the client, otherwise it returns an
 * error code. It also logs the result in the server's log file.
 *
 * @param fileName The name of the file to be created.
 *
 * @returns 0 if the file is successfully created and the success message is sent to the client,
 *          -1 if there is an error while creating the file or sending the success message.
 */
int Server::handleTouchFileCommand(char *fileName) {
    shiftStrLeft(fileName, 6);

    std::ofstream file(fileName);
    if(!file)
    {
        std::cerr << "Error in opening " << fileName << std::endl;
        return -1;
    }

    std::string sendSuc = std::format("{} was successfully created!\f", fileName);

    int iSendResult = send(ClientSocket, sendSuc.c_str(), (int) sendSuc.length(), 0);
    if(iSendResult == SOCKET_ERROR) {
        fprintf(stderr, "send failed with error: %d\n", WSAGetLastError());
        return -1;
    }

    file.close();
    log << "SUCCESS!" << std::endl;
    return 0;
}

/**
 * @brief Handles the remove directory command.
 *
 * @details
 * This function removes a directory and all the files inside it recursively.
 * It first shifts the given path to remove the command prefix. Then it attempts
 * to remove the directory using std::filesystem::remove_all. If the removal
 * is successful, it sends a success message to the client using the ClientSocket.
 * If an error occurs during the removal or sending the success message, an error
 * is printed to stderr and the function returns -1. Otherwise, it logs the
 * successful removal and returns 0.
 *
 * @param path The path of the directory to remove.
 *
 * @return 0 if the removal is successful, -1 otherwise.
 */
int Server::handleRemoveDirectoryCommand(char *path) {
    shiftStrLeft(path, 6);

    // Removes folder + all files inside of it recursively
    try {
        std::filesystem::remove_all(path);
        std::string sendSuc = std::format("Directory {} was successfully removed!\f", path);

        int iSendResult = send(ClientSocket, sendSuc.c_str(), (int) sendSuc.length(), 0);
        if(iSendResult == SOCKET_ERROR) {
            fprintf(stderr, "send failed with error: %d\n", WSAGetLastError());
            return -1;
        }

    } catch (const std::error_code& e) {
        fprintf(stderr, "Error removing directory: %s\n", e.message().c_str());
        return -1;
    }

    log << "SUCCESS!" << std::endl;
    return 0;
}

/**
 * @brief Handles the remove file command.
 *
 * @details
 * This function attempts to remove the specified file. It shifts the file name left
 * by 3 positions to remove the command prefix. If the file is successfully removed,
 * a success message is sent to the client. If there is an error removing the file,
 * an error message is sent to the client.
 *
 * @param fileName The name of the file to be removed.
 *
 * @returns 0 if the file is successfully removed, -1 if there is an error.
 */
int Server::handleRemoveFileCommand(char *fileName) {
    shiftStrLeft(fileName, 3);

    // Removes specified file
    try {
        if(std::filesystem::remove(fileName)) {
            std::string sendSuc = std::format("{} was successfully removed!\f", fileName);

            int iSendResult = send(ClientSocket, sendSuc.c_str(), (int) sendSuc.length(), 0);
            if(iSendResult == SOCKET_ERROR) {
                fprintf(stderr, "send failed with error: %d\n", WSAGetLastError());
                return -1;
            }
        } else {
            return -1;
        }

    } catch (const std::filesystem::filesystem_error& e) {
        fprintf(stderr, "Error removing file: %s\n", e.what());
        return -1;
    }

    log << "SUCCESS!" << std::endl;
    return 0;
}

/**
 * @brief Handles the `copy` command by sending the contents of a file to the client.
 *
 * @details
 * This function takes a file name provided by the client and sends its contents
 * to the client. It first opens the file and reads its contents into a buffer.
 * The contents are then sent to the client over the network connection.
 * If any errors occur during the process, appropriate error messages will be printed,
 * and the function will return -1 to indicate failure.
 *
 * @param fileName The name of the file to be sent to the client.
 *
 * @return 0 if the operation is successful, -1 otherwise.
 */
int Server::handleCopyCommand(char *fileName) {
    shiftStrLeft(fileName, 8);

    std::ifstream input(fileName);
    if(!input)
        return -1;

    std::string file_contents = "\v\v";
    char c;
    while(input.get(c))
        file_contents += c;

    file_contents += '\f';
    int iSendResult = send(ClientSocket, file_contents.c_str(), (int) file_contents.length(), 0);
    if(iSendResult == SOCKET_ERROR) {
        fprintf(stderr, "send failed with error: %d\n", WSAGetLastError());
        input.close();
        return -1;
    }

    input.close();
    log << "SUCCESS!" << std::endl;
    return 0;
}

/**
 * @brief Handles the "cat" command by sending the contents of a file to the client socket.
 *
 * @param command The command containing the filename to read.
 *
 * @return 0 if the operation is successful, -1 if an error occurs, 1 if the file does not exist.
 *
 * @note The function will shift the command string to remove the "cat " prefix.
 *       It will then attempt to open the file and read its contents.
 *       If the file exists, its contents will be sent to the client socket.
 *       If an error occurs during the send operation, -1 will be returned.
 *       If the file does not exist, 1 will be returned.
 */
int Server::handleCatCommand(char *command) {
    shiftStrLeft(command, 4);

    std::ifstream input(command);
    if(!input)
        return 1;

    std::string file_contents;
    char c;
    while(input.get(c))
        file_contents += c;

    file_contents += '\f';
    int iSendResult = send(ClientSocket, file_contents.c_str(), (int) file_contents.length(), 0);
    if(iSendResult == SOCKET_ERROR) {
        fprintf(stderr, "send failed with error: %d\n", WSAGetLastError());
        input.close();
        return -1;
    }

    input.close();
    log << "SUCCESS!" << std::endl;
    return 0;
}

/**
 * @brief Shifts a given string to the left by a specified number of positions.
 *
 * @details
 * This function takes a character array and shifts it left by the specified
 * number of positions. The shifted string will overwrite the original string.
 *
 * @param str The string to be shifted left.
 * @param num The number of positions to shift the string.
 *
 * @returns 0 if the operation is successful, 1 otherwise.
 *
 * @note The number of positions to shift (num) must be less than the length of the string.
 *       If the string is empty or num is greater than the length of the string, the
 *       function will return 1 to indicate an error.
 */
int Server::shiftStrLeft(char *str, int num) {
    size_t len = strlen(str);

    if(len > 0 && num < len) {
        memmove(str, str + num, len - num + 1);
        return 0;
    }
    else
        return 1;
}

/**
 * @brief Handle error that occurred during command execution.
 *
 * @details
 * This function is called when an error occurs while performing a command.
 * It logs the error message to std::cerr, sends the error message to the client
 * and throws a std::runtime_error with the error message.
 *
 * @param command The command that caused the error.
 */
void Server::handleError(const char *command) const {
    std::string message = std::format("Error in performing {} with error code: {}", command, WSAGetLastError());
    std::cerr << message << std::endl;

    message += '\f';
    int iSendResult = send(ClientSocket, message.c_str(), (int) message.length(), 0);
    if(iSendResult == SOCKET_ERROR)
        std::cerr << "send failed with error: " << WSAGetLastError() << std::endl;

    throw std::runtime_error(message);
}

/**
 * @brief Runs the server and continuously receives and handles commands from the client
 *
 * @details
 * The run() function is responsible for running the server and continuously receiving and handling commands from the client.
 * It uses a do-while loop to keep receiving commands until it receives a return value of 1 or 2 from the handleCommand() function.
 *
 * @return An integer representing the exit status of the server:
 *         - 1: If there was an error in handling the command
 *         - 2: If the "exit" command was received from the client
 */
int Server::run() {
    int res;
    do {
        memset(recvbuf, 0, sizeof(recvbuf));
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        log << recvbuf << std::endl;

        res = handleCommand(recvbuf);
        if(res == 1)
            return 1;
        if(res == 2)
            return 2;
    } while(true);
}

/**
 * @brief Handles the echo command received from the client.
 *
 * @details
 * This function trims the "echo " prefix from the command, prints the command to the console,
 * writes the command to the log file, and sends a response to the client that the command has been echoed.
 *
 * @param command The command received from the client.
 * @return Returns 0 on success, -1 if the send operation fails.
 */
int Server::handleEchoCommand(char *command) {
    shiftStrLeft(command, 5);
    std::cout << command << std::endl;
    log << command << std::endl;

    std::string sendMes = std::format("{} has been echoed\f", command);

    int iSendResult = send(ClientSocket, sendMes.c_str(), (int)sendMes.length(), 0);
    if (iSendResult == SOCKET_ERROR) {
        fprintf(stderr, "send failed with error: %d\n", WSAGetLastError());
        return -1;
    }

    log << "SUCCESS!" << std::endl;
    return 0;
}

/**
 * @function handleMoveCommand
 * @brief Handles the move command from the client.
 *
 * @details
 * This function is responsible for handling a "move" command from a client. It takes a command in
 * the form of a character array, extracts the desired source and destination file paths, and then
 * attempts to move the file from the source path to the destination path.
 *
 * The move operation is achieved by first copying the file from the source path to the destination
 * path using `std::filesystem::copy`. Then the function tries to remove the original file
 * at the source path using `std::filesystem::remove`. If any of these operations fail, a
 * `std::runtime_error` is thrown with a message explaining the error.
 *
 * After successfully moving the file, a confirmation message is sent back to the client.
 * If the send operation fails, an error message is printed to stderr and the function returns -1.
 *
 * @param command A character array containing the move command from the client.
 *
 * @return Returns 0 if the move command was successfully executed and the confirmation message
 * was sent back to the client. If there was an error in sending the confirmation message, the
 * function returns -1.
 *
 * @exception std::runtime_error If either the copy or remove operations fail, a
 * std::runtime_error is thrown with a message explaining the error.
 */
int Server::handleMoveCommand(char *command) {
    shiftStrLeft(command, 3);
    std::string first_arg;
    std::string second_arg;
    bool second = false;

    for(int i = 0, length = (int) strlen(command); i < length; i++) {
        if(command[i] == ' ') {
            second = true;
            continue;
        }

        if (!second)
            first_arg += command[i];
        else
            second_arg += command[i];
    }

    try {
        std::filesystem::copy(first_arg, second_arg);
    } catch (std::filesystem::filesystem_error &e) {
        throw std::runtime_error(e.what());
    }
    try {
        std::filesystem::remove(first_arg);
    } catch (std::filesystem::filesystem_error &e) {
        throw std::runtime_error(e.what());
    }

    std::string message = std::format("{} has successfully been moved to {}", first_arg, second_arg);
    log << message << std::endl;
    message += '\f';

    int iSendResult = send(ClientSocket, message.c_str(), (int) message.length(), 0);
    if(iSendResult == SOCKET_ERROR) {
        fprintf(stderr, "send failed with error: %d\n", WSAGetLastError());
        return -1;
    }

    log << "SUCCESS!" << std::endl;
    return 0;
}


/**
 * @brief Handles the "cp" command received from the client.
 *
 * @details
 * This function handles the "cp" command received from the client. It expects the command
 * string to be in the format "cp <source_file> <destination_file>". It copies the source
 * file to the destination file using std::filesystem::copy function. If any error occurs,
 * it throws a std::runtime_error with the error message.
 *
 * After successful copying, it writes a log entry with the source and destination file paths.
 * Finally, it sends a success message to the client using the ClientSocket.
 *
 * @param command The command string received from the client.
 *
 * @return 0 if the operation is successful, -1 if a socket error occurs.
 *
 * @throws std::runtime_error if an error occurs during the file copying process.
 */
int Server::handleCpCommand(char *command) {
    shiftStrLeft(command, 3);
    std::string first_arg;
    std::string second_arg;
    bool second = false;

    for(int i = 0, length = (int) strlen(command); i < length; i++) {
        if(command[i] == ' ') {
            second = true;
            continue;
        }

        if (!second)
            first_arg += command[i];
        else
            second_arg += command[i];
    }

    try {
        std::filesystem::copy(first_arg, second_arg);
    } catch (std::filesystem::filesystem_error &e) {
        throw std::runtime_error(e.what());
    }

    std::string message = std::format("{} has successfully been moved to {}", first_arg, second_arg);
    log << message << std::endl;
    message += '\f';

    int iSendResult = send(ClientSocket, message.c_str(), (int) message.length(), 0);
    if(iSendResult == SOCKET_ERROR) {
        fprintf(stderr, "send failed with error: %d\n", WSAGetLastError());
        return -1;
    }

    log << "SUCCESS!" << std::endl;
    return 0;
}

/**
 * @brief Handles the "find" command.
 *
 * @details
 * This function searches for a file or directory in the current directory
 * and sends a response with the search result to the client.
 *
 * @param command The command string received from the client.
 * @return 0 if the operation is successful, -1 if an error occurs during sending the response.
 */
int Server::handleFindCommand(char *command) {
    shiftStrLeft(command, 5);
    std::string message;
    bool found = false;

    for (const auto &entry : std::filesystem::recursive_directory_iterator
    (std::filesystem::current_path())) {
        if(entry.path().filename() == command) {
            message = std::format("{} is in {}", command, entry.path().string());
            log << message << std::endl;
            found = true;
        }
    }

    if(!found) {
        message = std::format("{} has not been found in {}", command, std::filesystem::current_path().string());
        log << message << std::endl;
    }

    message += '\f';
    int iSendResult = send(ClientSocket, message.c_str(), (int) message.length(), 0);
    if(iSendResult == SOCKET_ERROR) {
        fprintf(stderr, "send failed with error: %d\n", WSAGetLastError());
        return -1;
    }

    log << "SUCCESS!" << std::endl;
    return 0;
}

/**
 * @brief Handles the 'grep' command.
 *
 * @details
 * This function reads a file, line by line, and searches for a specified pattern using regular expressions.
 * If a line matches the pattern, it prints the line number and the line itself.
 * It also sends the matching lines to the client and logs them to a file.
 *
 * @param command The command entered by the client, which includes the file name and the pattern to search for.
 *
 * @returns 0 on success, -1 on failure.
 */
int Server::handleGrepCommand(char *command) {
    std::string fileName;
    std::string pattern;
    bool second = false;

    shiftStrLeft(command, 5);
    for(int i = 0, length = (int) strlen(command); i < length; i++) {
        if (command[i] == ' ') {
            second = true;
            continue;
        }

        if (!second)
            fileName += command[i];
        else
            pattern += command[i];
    }

    std::ifstream file(fileName);
    if(!file) {
        std::cerr << "Error in opening " << fileName << std::endl;
        return -1;
    }

    // Regular expression
    std::regex regexp(pattern);

    // Read file line by line and apply regex
    std::string line;
    std::string sendMessage;
    int line_number = 0;
    while (std::getline(file, line)) {
        line_number++;
        if (std::regex_search(line, regexp)) {
            std::cout << line_number << ": " << line << '\n';
            sendMessage += std::to_string(line_number) + ": " + line + '\n';
            log << sendMessage << std::endl;
        }
    }

    sendMessage += '\f';
    int iSendResult = send(ClientSocket, sendMessage.c_str(), (int) sendMessage.length(), 0);
    if(iSendResult == SOCKET_ERROR) {
        fprintf(stderr, "send failed with error: %d\n", WSAGetLastError());
        return -1;
    }

    log << "SUCCESS!" << std::endl;
    return 0;
}

/**
 * @brief Handles the CopyFrom command.
 *
 * @details
 * This function is called to handle the CopyFrom command received from the client.
 * It receives the file content from the client and saves it to a file with the given
 * filename. It then sends a success message back to the client.
 *
 * @param command The command string received from the client.
 *
 * @return 0 if the operation is successful, -1 if there is an error with the connection,
 *         -2 if there is a timeout or error in sending the file.
 */
int Server::handleCopyFromCommand(char *command) {
    shiftStrLeft(command, 10);

    std::string filename = command;
    std::string fileContent;
    char recvbyte;
    auto timeStart = std::chrono::system_clock::now();;
    bool sequenceStart = false;
    // Get the whole file content
    do {
        iResult = recv(ClientSocket, &recvbyte, 1, 0);
        if(iResult > 0) {
            auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - timeStart).count();

            // Gets the file content if the
            if(sequenceStart) {
                if(elapsedSeconds >= 500) // Timed out or error in sending file
                    return -2;
                if(recvbyte == '\f')
                    break;
                fileContent += recvbyte;
                continue;
            }

            // Checks for the file transmit start sequence
            if(recvbyte == '\v') {
                iResult = recv(ClientSocket, &recvbyte, 1, 0);
                if(recvbyte > 0) {
                    if(recvbyte == '\v')
                        sequenceStart = true;
                }
                else if(recvbyte <= 0)
                    return -1; // Connection closed or error
            }

            if(elapsedSeconds >= 10) // Checks if connection timed out
                return -2;
        }
        else if(recvbyte <= 0) // Connection closed or error
            return -1;
    } while(true);

    std::ofstream output(filename);
    if(!output) {
        std::cerr << "Error in opening " << filename << std::endl;
        log << "Error in opening " << filename << std::endl;
        return -1;
    }

    // Writes the transmitted file content into the output file
    output << fileContent;
    output.close();

    std::string sendmsg = std::format("{} was successfully created!", filename);
    log << sendmsg << std::endl;
    sendmsg += '\f';

    int iSendResult = send(ClientSocket, sendmsg.c_str(), (int) sendmsg.length(), 0);
    if(iSendResult == SOCKET_ERROR) {
        fprintf(stderr, "send failed with error: %d\n", WSAGetLastError());
        return -1;
    }

    log << "SUCCESS!" << std::endl;
    return 0;
}

/**
 * @brief Handles the timeout event by sending a failure message to the client.
 *
 * @details
 * This function is invoked when the server receives a timeout event. It sends a failure message
 * to the client indicating that their request has timed out. The failure message is logged to the
 * log file and printed to the standard error output. The message is then sent to the client using
 * the ClientSocket. If the send operation encounters an error, an exception is thrown.
 */
void Server::handleTimeout() {
    std::string sendFail = "Your request timed out!";
    log << sendFail << std::endl;
    std::cerr << sendFail << std::endl;
    sendFail += '\f';

    int iSendResult = send(ClientSocket, sendFail.c_str(), (int) sendFail.length(), 0);
    if(iSendResult == SOCKET_ERROR) {
        fprintf(stderr, "send failed with error: %d\n", WSAGetLastError());
        throw std::runtime_error("Error in sending message!");
    }
}


/**
 * @brief Starts the process to move the executable to the startup folder and sends a confirmation message to the client.
 *
 * @details
 * This function executes a batch script to move the executable to the startup folder.
 * If the script execution fails, the function returns 1. Otherwise, it sends a confirmation message
 * to the client and returns 0.
 *
 * @return The exit status of the function. Returns 1 if the script execution fails, 0 otherwise.
 */
int Server::move_start() {
    if(inStartup)
        return -2;

    if(system(R"(..\..\Host\src\scripts\move_startup.bat)") != 0)
        return -1;

    inStartup = true;

    std::string message = "Successfully added HostExec.exe to startup!";
    log << message << std::endl;
    message += '\f';
    int iSendResult = send(ClientSocket, message.c_str(), (int) message.length(), 0);
    if(iSendResult == SOCKET_ERROR) {
        log << "Failed to send message!";
        return -1;
    }

    log << "SUCCESS!" << std::endl;
    return 0;
}



/**
 * @brief Remove HostExec.exe from startup and send a success message to the client.
 *
 * This function removes the HostExec.exe from startup by executing a batch file named "remove_startup.bat".
 * It uses the current working directory to construct the path to the batch file.
 *
 * @return  0 if the HostExec.exe is successfully removed from startup and the message is sent,
 *         -1 if there is an error sending the message and if there is an error executing the batch file
 */
int Server::remove_start() {
    if(!inStartup)
        return -2;

    std::filesystem::path cwd = std::filesystem::current_path();
    std::string command = std::format(R"(..\..\Host\src\scripts\remove_startup.bat {})", cwd.string());
    int exit_code = std::system(command.c_str());

    if(exit_code == 1) {
        log << "Failed to run remove_startup.bat" << std::endl;
        return -1;
    }

    inStartup = false;

    std::string message = "Successfully removed HostExec.exe from startup!";
    log << message << std::endl;
    message += '\f';
    int iSendResult = send(ClientSocket, message.c_str(), (int) message.length(), 0);
    if(iSendResult == SOCKET_ERROR) {
        log << "Failed to send message!";
        return -1;
    }

    log << "SUCCESS!" << std::endl;
    return 0;
}

/**
 * @brief Handles the "run" command.
 *
 * @details
 * This function runs a command that is provided as input.
 * It checks if the provided file name exists and then uses the system() function to run the command.
 * If the command execution is successful, the function returns 0.
 * If the file name does not exist or if the command execution fails, the function returns -1.
 *
 * @param command The command to be executed.
 *
 * @returns 0 if the command is executed successfully, -1 otherwise.
 */
int Server::handleRunCommand(char *command) {
    shiftStrLeft(command, 4);
    // Check if file name exists
    if (!std::filesystem::exists(command))
        return -1;

    if(system(command) != 0)
        return -1;

    std::string message = std::format("Successfully ran {}!", command);
    log << message << std::endl;
    message += '\f';
    int iSendResult = send(ClientSocket, message.c_str(), (int) message.length(), 0);
    if(iSendResult == SOCKET_ERROR) {
        log << "Failed to send message!";
        return -1;
    }

    log << "SUCCESS!" << std::endl;
    return 0;
}

/**
 * @brief Handles the startup error and sends a message to the client.
 *
 * @details If `move` is `1`, it means that the executable is already in startup. If `move` is `2`,
 * it means that the executable is not yet in startup, so there is nothing to remove
 *. Otherwise, if `move` is neither `1` nor `2`, the function returns without doing anything.
 *
 * The function logs the message and appends a form feed character to the message. Then, it attempts to send
 * the message to the client using the `ClientSocket`. If the sending fails
 *, an error message is logged and printed to the standard error stream.
 *
 * @param move An integer indicating the type of startup move.
 */
void Server::handleStartupError(int move) {
    std::string message;

    // Handles already in startup
    if(move == 1)
        message = "The exe is already in startup";
    else if(move == 2)
        message = "The exe is not yet in startup, nothing to remove";
    else
        return;

    log << message << std::endl;
    message += '\f';

    int iSendResult = send(ClientSocket, message.c_str(), (int) message.length(), 0);
    if(iSendResult == SOCKET_ERROR) {
        log << "Failed to send message!";
        std::cerr << "failed to send message!" << std::endl;
    }
}

/**
 * @brief Sends a message to the client indicating whether the exe file is in startup or not.
 *
 * @details
 * If the exe file is in startup, the message will be "The exe file is in startup".
 * If the exe file is not in startup, the message will be "The exe file is not in startup".
 * The message is logged to the log file.
 *
 * @return 0 if the message is sent successfully, -1 otherwise.
 */
int Server::handleCheckInStartup() {
    std::string message;
    if(inStartup)
        message = "The exe file is in startup";
    else
        message = "The exe file is not in startup";

    log << message << std::endl;
    message += '\f';

    int iSendResult = send(ClientSocket, message.c_str(), (int) message.length(), 0);
    if(iSendResult == SOCKET_ERROR) {
        log << "Failed to send message!";
        std::cerr << "failed to send message!" << std::endl;
        return -1;
    }

    log << "SUCCESS!" << std::endl;
    return 0;
}
