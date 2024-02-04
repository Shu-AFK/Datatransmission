#ifndef DATATRANSMISSION_SERVER_H
#define DATATRANSMISSION_SERVER_H

/*
ls - List all files and directories in the current directory. //
cd - Change current directory. //
pwd - Print working directory. //
cat - Concatenate and display the content of files. //
echo - Output the inputs. //
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


#define WIN32_LEAN_AND_MEAN

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
    static void handleExitCommand();
    int handleChangeDirectoryCommand(const char* path);
    int handleLsCommand(char *command);
    int sendCmdDoesntExist();
    int handleMakeDirectoryCommand(char *path);
    int handleTouchFileCommand(char *fileName);
    int handleRemoveDirectoryCommand(char *path);
    int handleRemoveFileCommand(char *fileName);
    int handleCopyCommand(char *fileName);
    int handleCatCommand(char *command);
    int handleEchoCommand(char *command);
    int handleMoveCommand(char *command);

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

    int run();
};

#endif //DATATRANSMISSION_SERVER_H
