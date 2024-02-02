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
    Client(const std::string& server_name, const std::string& port = DEFAULT_PORT) {
        ConnectSocket = INVALID_SOCKET;
        this->port = port;
        log.open("log.txt");
        if(!log)
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

    // communication loop
    void run() {
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

private:
    void initWinsock() {
        iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0) {
            throw std::runtime_error("WSAStartup failed");
        }
    }

    void initServerConnection(const char *server_name) {
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

    SOCKET createAndConnectSocket() {
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

    static int shiftStrLeft(std::string &str, int num) {
        size_t len = str.length();

        if(len > 0 && num < len) {
            str = str.substr(num);
            return 0;
        }
        else
            return 1;
    }

    static int sendData(SOCKET clientSocket, const std::string& cmd)
    {
        int iSendResult = send(clientSocket, cmd.c_str(), (int) cmd.length(), 0);
        if(iSendResult == SOCKET_ERROR)
        {
            std::cout << "Error in sending command to server. Error: " << WSAGetLastError() << std::endl;
            return -1;
        }

        return 0;
    }

    static std::string recvData(SOCKET clientSocket, std::string& cmd) {
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
};

int _cdecl main(int argc, char **argv)
{
    // Validate the parameters
    if (argc != 2) {
        printf("usage: %s server-name [optional port]\n", argv[0]);
        return 1;
    }

    Client client(argv[1]);
    try {
        client.run();
    } catch(std::runtime_error &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}