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
#include <sstream>
#include <chrono>
#include <sqlite3.h>
#include <unordered_map>
#include <sodium.h>

class Server {
private:
    static constexpr const int DEFAULT_BUFLEN = 512;
    SOCKET ClientSocket = INVALID_SOCKET;
    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET LastSock = INVALID_SOCKET;
    std::ofstream log;
    std::fstream settings;
    WSADATA wsaData;
    std::string port;
    std::filesystem::path settingsPath;
    bool inStartup = false;
    int iResult;
    struct addrinfo* result = nullptr, * ptr = nullptr, hints;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    std::string db_name = "users.db";
    sqlite3* DB;
    std::unordered_map<SOCKET, std::string> userMap;

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

    // Misc functions
    static int shiftStrLeft(char* str, int num);
    int handleSend(std::string sen, SOCKET sock);
    void handleError(const char* command);
    int handleCommand(char* command);
    void handleTimeout();
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
    /**
     * @brief Server constructor.
     *
     * This constructor initializes the Server class and sets up the server.
     * It takes a string argument, `spec_port`, which specifies the port for the server to listen on.
     *
     * @param spec_port The port to listen on as a string.
     * @throws std::runtime_error if an error occurs when initializing the server.
     */
    Server(const std::string& spec_port) {
        if(sodium_init() < 0)
            throw std::runtime_error("Could not init sodium");

        settingsPath = "settings.txt";

        log.open("log.txt");
        if (!log)
            throw std::runtime_error("Failed to open log file");

        settings.open(settingsPath, std::ios::in);
        if (!settings.is_open()) {
            std::ofstream create_settings(settingsPath);
            create_settings.close();
        }

        std::string line;
        std::getline(settings, line);
        if (line.empty()) {
            settings.close();
            settings.open(settingsPath, std::ios::out);
            if (!settings.is_open())
                throw std::runtime_error("Failed to open settings file");
            settings << "startup = false";
        }
        else if (line == "startup = true")
            inStartup = true;
        else if (line == "startup = false")
            inStartup = false;

        settings.close();

        int rc = sqlite3_open(db_name.c_str(), &DB);

        if (rc) {
            std::string message = std::format("Can't open database: {}", sqlite3_errmsg(DB));
            throw std::runtime_error(message);
        }

        try {
            if (dbIsEmpty())
                initDB();
        }
        catch (const std::runtime_error& e) {
            throw e;
        }

        // Add root user
        if (addUser("root", "root") == -1)
            throw std::runtime_error("unable to insert root user into database!");

        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_flags = AI_PASSIVE;

        this->port = spec_port;

        if (!setupPort())
            throw std::runtime_error("Failed to init port");
        if (!initServer())
            throw std::runtime_error("Failed to start the server");

        memset(recvbuf, 0, DEFAULT_BUFLEN);
    }

    /**
     * @brief Destructor for the Server class.
     *
     * This destructor will clean up all the resources used by the Server object.
     * It closes the client socket, cleans up the Winsock API, frees the address info result,
     * closes the log file, updates the settings file, and closes the SQLite database.
     *
     * @throws std::runtime_error if failed to open the settings file
     */
    ~Server() { // Destructor will clean up all the resources correctly
        closesocket(ClientSocket);
        WSACleanup();
        freeaddrinfo(result);
        log.close();

        settings.open(settingsPath, std::ios::out);
        if (!settings.is_open())
            throw std::runtime_error("Failed to open settings file");

        if (inStartup)
            settings << "startup = true";
        else
            settings << "startup = false";

        settings.close();
        sqlite3_close(DB);
    }

    void run();
    int addUser(const std::string& name, const std::string& password);
    int remUser(const std::string& name);
    int addStartup();
    int setCwd(const std::string& path);

    int handleAuth(char* command);
};

#endif //DATATRANSMISSION_SERVER_H
