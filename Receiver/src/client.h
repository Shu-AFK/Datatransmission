#ifndef DATATRANSMISSION_CLIENT_H
#define DATATRANSMISSION_CLIENT_H

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

class Client {
    std::string port;
    WSADATA wsaData;
    SOCKET ConnectSocket;
    addrinfo *result, *ptr, hints;
    std::string recvbuf;
    std::ofstream log;
    int iResult;
    const static int recvbuflen = DEFAULT_BUFLEN;

public:
    Client(const std::string &server_name, const std::string &port = DEFAULT_PORT) {
        ConnectSocket = INVALID_SOCKET;
        this->port = port;
        log.open("log.txt");
        if (!log)
            throw std::runtime_error("Failed to open log file");
        initWinsock();
        initServerConnection(server_name.c_str());
    }

    ~Client() {
        // cleanup
        log.close();
        closesocket(ConnectSocket);
        WSACleanup();
    }

    void run();

private:
    void initWinsock();
    void initServerConnection(const char *server_name);
    SOCKET createAndConnectSocket();
    static int shiftStrLeft(std::string &str, int num);
    static int sendData(SOCKET clientSocket, const std::string& cmd);
    static std::string recvData(SOCKET clientSocket, std::string& cmd);
};

#endif //DATATRANSMISSION_CLIENT_H
