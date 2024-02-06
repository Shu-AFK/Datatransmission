/*
 *  Filename: server.h
 *  Author: Floyd
 *
 *  The `Server` class is designed to encapsulate all logic and data needed
 *  to manage a server in a client-server architecture. It implements
 *  functionality to manage network communication, command handling and error handling.
 *
 *  Private member variables:
 *  - DEFAULT_BUFLEN: Represents the default length for the receive buffer.
 *  - ClientSocket and ListenSocket: Used to manage connections.
 *  - log: Object to manage log file.
 *  - wsaData: WSADATA object required for the use of Winsock2 library.
 *  - port: String to store the port for the server to listen on.
 *  - iResult: Integer used to store result values.
 *  - result and ptr: Pointers to addrinfo structure for network communication management.
 *  - hints: An addrinfo structure, which is used in network communication setup.
 *  - recvbuf: Character buffer to store received data.
 *  - recvbuflen: Integer to store the receive buffer length.
 *
 *  Private member methods:
 *  - handlePwdCommand, handleExitCommand, handleChangeDirectoryCommand, handleLsCommand,
 *    sendCmdDoesntExist, handleMakeDirectoryCommand, handleTouchFileCommand,
 *    handleRemoveDirectoryCommand, handleRemoveFileCommand, handleCopyCommand, handleCatCommand,
 *    handleEchoCommand, handleMoveCommand, handleCpCommand: These methods are implemented
 *    to handle specific commands sent from a client to the server.
 *  - shiftStrLeft: Helper utility function for string manipulation.
 *  - handleError: Error handling methodology, encapsulated in a function.
 *  - handleCommand: Function to parse received commands and call respective command handlers.
 *  - initServer: Function to initialize server.
 *  - setupPort: Function to set up the port for the server to listen on.
 *
 *  Public member variables:
 *  - Constructor: Defines a constructor for the Server object, which takes a port number as an argument.
 *    It also configures server parameters and throws an exception if something goes wrong.
 *  - Destructor: Cleans up the resource used by the Server object such as closing sockets, freeing addrinfo, and cleaning up Winsock2 library.
 *  - run: The main loop for the server operation. Continuously listens for incoming data and handles them.
 */

#ifndef DATATRANSMISSION_SERVER_H
#define DATATRANSMISSION_SERVER_H

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
#include <regex>

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
    int handleCpCommand(char *command);
    int handleFindCommand(char *command);
    int handleGrepCommand(char *command);
    int handleCopyFromCommand(char *command);

    // Misc functions
    static int shiftStrLeft(char *str, int num);
    void handleError(const char *command) const;
    int handleCommand(char *command);
    void handleTimeout();

    static int move_start();
    int remove_start();

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
