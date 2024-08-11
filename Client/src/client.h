/*
* Filename: client.h
* Author: Floyd
*
* The `Client` class provides the functionality needed to interact with a server.
* This interaction includes sending commands and receiving responses from the server.
* The Client class also logs these interactions.
*
* Private member variables:
*  - port: A string that stores the port used for the client server connection.
*  - wsaData: A WSADATA object used for network communication.
*  - ConnectSocket: A SOCKET object used to manage the connection to the server.
*  - result and ptr: Pointers to an addrinfo structure used for obtaining address information.
*  - hints: An addrinfo structure that is used in network communication setup.
*  - recvbuf: A string buffer to store received data.
*  - log: An ofstream object to handle logging.
*  - iResult: An integer used to store result values.
*  - recvbuflen: An integer constant to store the receive buffer length.
*
* Private member methods:
*  - initWinsock: Function that initializes Winsock.
*  - initServerConnection: Function that initializes the server connection.
*  - createAndConnectSocket: Function that creates and connects a socket.
*  - shiftStrLeft: Helper utility function for string manipulation.
*  - sendData: Function that sends data to the server.
*  - recvData: Function that receives data from the server.
*
* Public member variables:
*  - Constructor: Defines a constructor for the Client object which takes a server name and port as arguments.
*    It also initializes the Winsock and server connection and throws an exception if an error occurs.
*  - Destructor: Cleans up the resources used by the Client object: closes the log file, the connect socket and cleans up the Winsock.
*  - run: The main loop for the client operation. Engages in command/response interactions with the server.
*/

#ifndef DATATRANSMISSION_CLIENT_H
#define DATATRANSMISSION_CLIENT_H

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <fstream>
#include <utility>
#include <cstdio>

#define DEFAULT_BUFLEN 512

class Client {
private:
    void initWinsock();
    void initServerConnection(const char *server_name);
    SOCKET createAndConnectSocket();
    static int shiftStrLeft(std::string &str, int num);
    int sendData(SOCKET clientSocket, std::string cmd);
    static std::string recvData(SOCKET clientSocket, std::string cmd);

    WSADATA wsaData;
    SOCKET ConnectSocket;
    addrinfo *result, *ptr, hints;
    std::string recvbuf;
    std::ofstream log;
    int iResult;
    const static int recvbuflen = DEFAULT_BUFLEN;
    std::string ip, port, username, password;
    std::string None;

    // For GUI use
#ifdef DATATRANSMISSION_CLIENT_GUI
    std::string content;
#endif

    static std::string getLogFilename();

public:
    Client(std::string ip, std::string port, std::string username, std::string password)
            : ip(ip), port(port), username(username), password(password) {

        // Log before moving parameters
        log.open(getLogFilename());
        if (!log) {
            throw std::runtime_error("Failed to open log file");
        }
        log << "IP: " << ip << " Port: " << port << " Username: " << username << std::endl;

        // Now move parameters
        this->ip = std::move(ip);
        this->port = std::move(port);
        this->username = std::move(username);
        this->password = std::move(password);

        ConnectSocket = INVALID_SOCKET;

        try {
            initWinsock();
            initServerConnection(this->ip.c_str());
        } catch (const std::runtime_error &e) {
            log << e.what() << std::endl;
            throw e;
        }
    }

    ~Client() {
        // cleanup
        log.close();
        closesocket(ConnectSocket);
        WSACleanup();
    }

    void run();
    void closeConnection();
    int sendCommand(std::string command);

#ifdef DATATRANSMISSION_CLIENT_GUI
    std::string getBuffer();
    std::string getIP();
    static std::string insertTabAfterNewline(const std::string &input);
#endif
};

#endif //DATATRANSMISSION_CLIENT_H
