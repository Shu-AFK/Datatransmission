#define WIN32_LEAN_AND_MEAN

/*
ls - List all files and directories in the current directory. //
cd - Change current directory. //
pwd - Print working directory. //
cat - Concatenate and display the content of files.
echo - Output the inputs.
mkdir - Create a new directory. //
rmdir - Removes a directory. //
rm - Remove files or empty directories. //
touch - Create an empty file. //
mv - Move or rename files or directories.
cp - Copy files or directories.
find - Search for files in a directory hierarchy.
grep - Search text using patterns.
exit - Exit the shell. //
copy_pc - copy's a file to the client pc //
*/

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdio>
#include <fstream>
#include "helper.h"
#include <direct.h>
#include <filesystem>
#include <iostream>
#include <format>

static constexpr const char* DEFAULT_PORT = "27015";

class Server {
private:
    static constexpr const int DEFAULT_BUFLEN = 512;
    SOCKET ClientSocket = INVALID_SOCKET;
    SOCKET ListenSocket = INVALID_SOCKET;
    std::ofstream log;
    WSADATA wsaData;
    std::string port;
    int iResult;
    struct addrinfo *result = nullptr, *ptr = nullptr, hints;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    int handlePwdCommand();
    void handleExitCommand();
    int handleChangeDirectoryCommand(const char* path);
    int handleLsCommand(char *command);
    int sendCmdDoesntExist();
    int handleMakeDirectoryCommand(char *path);
    int handleTouchFileCommand(char *fileName);
    int handleRemoveDirectoryCommand(char *path);
    int handleRemoveFileCommand(char *fileName);
    int handleCopyCommand(char *fileName);

    // Misc functions
    static int shiftStrLeft(char *str, int num);
    void handleError(const char *command) const;
    int handleCommand(char *command);

    bool initServer();
    bool setupPort();

public:
    Server(const std::string& spec_port) {
        log.open("log.txt");
        if(!log)
            throw std::runtime_error("Failed to open log file");

        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_flags = AI_PASSIVE;

        this->port = spec_port;

        if(!setupPort())
            throw std::runtime_error("Failed to init port");
        if(!initServer())
            throw std::runtime_error("Failed to start the server");

        memset(recvbuf, 0, DEFAULT_BUFLEN);
    }

    ~Server() { // Destructor will clean up all the resources correctly
        closesocket(ClientSocket);
        WSACleanup();
        freeaddrinfo(result);
        log.close();
    }

    void run();
};

int Server::handleCommand(char* command) {
    if (strncmp(command, "pwd", 3) == 0) {
        if(handlePwdCommand() == -1) {
            handleError("pwd");
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
    else if (strncmp(command, "copy_pc ", 8) == 0) {
        if(handleCopyCommand(command) == -1){
            handleError("copy_pc");
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

bool Server::setupPort(){
    // Creates helper scripts
    if(create_scripts(port.c_str()) != 0) {
        log << "Failed to create scripts" << std::endl;
        return false;
    }
    if(run_init() != 0) {
        log << "Failed to run scripts" << std::endl;
        return false;
    }

    return true;
}

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

void Server::handleExitCommand() {
    printf("Closing connection...\n");
}

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

int Server::shiftStrLeft(char *str, int num) {
    size_t len = strlen(str);

    if(len > 0 && num < len) {
        memmove(str, str + num, len - num + 1);
        return 0;
    }
    else
        return 1;
}

void Server::handleError(const char *command) const {
    std::string message = std::format("Error in performing {} with error code: {}", command, WSAGetLastError());
    std::cerr << message << std::endl;

    message += '\f';
    int iSendResult = send(ClientSocket, message.c_str(), (int) message.length(), 0);
    if(iSendResult == SOCKET_ERROR)
        std::cerr << "send failed with error: " << WSAGetLastError() << std::endl;

    throw std::runtime_error(message);
}

void Server::run() {
    int res;
    do {
        memset(recvbuf, 0, sizeof(recvbuf));
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        log << recvbuf << std::endl;

        res = handleCommand(recvbuf);
        if(res == 1)
            return;
    } while(true);
}

int main(int argc, char **argv) {
    std::string port;
    if(argc == 2)
        port = argv[1];
    else
        port = DEFAULT_PORT;

    Server server(port);

    try {
        server.run();
    }

    catch (const std::runtime_error &e) {
        std::ofstream log("log.txt", std::ios::app);
        log << e.what() << std::endl;
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}