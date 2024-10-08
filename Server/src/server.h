/* https://learn.microsoft.com/en-us/windows/win32/winsock/using-secure-socket-extensions */

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

#if !(defined(WIN32)) && !defined(WIN64)
#   error Only windows is supported.
#endif

#define WIN32_LEAN_AND_MEAN

#ifndef UNICODE
#define UNICODE
#endif

#include "helper.h"

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include <psapi.h>

#include <sqlite3.h>
#include <sodium.h>

#include <cstdio>
#include <fstream>
#include <direct.h>
#include <filesystem>
#include <iostream>
#include <format>
#include <regex>
#include <sstream>
#include <chrono>
#include <unordered_map>

class Server {
private:
    // Constants
    static constexpr const int DEFAULT_BUFLEN = 512;

    // Socket information
    struct addrinfo* result = nullptr, * ptr = nullptr, hints;
    std::unordered_map<SOCKET, std::string> userMap;
    SOCKET ClientSocket = INVALID_SOCKET;
    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET LastSock = INVALID_SOCKET;
    WSADATA wsaData;
    std::string port;

    // Files
    std::ofstream log;
    std::fstream settings;
    std::filesystem::path settingsPath;

    // Program specific booleans
    bool inStartup = false;

    // Received data
    int iResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    // Databse
    std::string db_name = "users.db";
    sqlite3* DB;

    int handlePwdCommand();
    static void handleExitCommand();
    int handleChangeDirectoryCommand(const char* path);
    int handleLsCommand(char* command);
    int sendCmdDoesntExist();
    int handleMakeDirectoryCommand(char* path);
    int handleTouchFileCommand(char* fileName);
    int handleRemoveDirectoryCommand(char* path);
    int handleRemoveFileCommand(char* fileName);
    int handleCopyCommand(char* fileName);
    int handleCatCommand(char* command);
    int handleEchoCommand(char* command);
    int handleMoveCommand(char* command);
    int handleCpCommand(char* command);
    int handleFindCommand(char* command);
    int handleGrepCommand(char* command);
    int handleCopyFromCommand(char* command);
    int handleRunCommand(char* command);
    int handleCheckInStartup();
    int handleCutCommand(char* command);
    int handleGetProcesses();

    // Misc functions
    static int shiftStrLeft(char* str, int num);
    int handleSend(std::string sen, SOCKET sock);
    void handleError(const char* command);
    int handleCommand(char* command);
    void handleStartupError(int move);
    void handleWrongUsage(const char* command);

    int move_start();
    int remove_start();

    bool initServer();
    bool setupPort();

    // Database
    int initDB();
    static int callback(void* instance, int argc, char** argv, char** azColName);
    int callbackImpl(int argc, char** argv, char** azColName);
    bool dbIsEmpty();
    int handleSQL(int rc, const char *zErrMsg, const char *operation);
    int auth(const std::string &username, const std::string &password);
    static std::vector<unsigned char> hash_pass(const std::string &password);

public:
    explicit Server(const std::string& spec_port);
    ~Server();

    int run();
    int addUser(const std::string& name, const std::string& password);
    int remUser(const std::string& name);
    int addStartup();
    int setCwd(const std::string& path);

    int handleAuth(char* command);
};

#endif //DATATRANSMISSION_SERVER_H
