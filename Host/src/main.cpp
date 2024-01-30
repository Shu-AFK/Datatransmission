/* SOURCE: https://learn.microsoft.com/en-us/windows/win32/winsock/complete-server-code
 * READ: https://learn.microsoft.com/en-us/training/modules/build-a-tcp-echo-client/?source=recommendations */

// TODO: Add error sending to client

#undef UNICODE

#define WIN32_LEAN_AND_MEAN

// Needed Libs
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdio>
#include <fstream>
#include "helper.h"
#include <direct.h>
#include <filesystem>
#include <iostream>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

int shiftStrLeft(char *str, int num);

int __cdecl main(void)
{
    // Creates helper scripts
    if(create_scripts(DEFAULT_PORT) != 0)
        return 1;

    // Opens the default port
    if(run_init() != 0)
        return 1;

    // Setup
    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    // Init Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if(iResult != 0) {
        fprintf(stderr, "WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    struct addrinfo *result = NULL, *ptr = NULL, hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if(iResult != 0) {
        fprintf(stderr, "getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for the server to listen for client connections
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if(ListenSocket == INVALID_SOCKET) {
        fprintf(stderr, "socket failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Set up the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if(iResult == SOCKET_ERROR) {
        fprintf(stderr, "bind failed with error: %d", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if(iResult == SOCKET_ERROR) {
        fprintf(stderr, "listen failed with error: %d", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // Accept a client socket
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if(ClientSocket == INVALID_SOCKET) {
        fprintf(stderr, "accept failed with error: %d", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    closesocket(ListenSocket);

    std::string cwd;
    bool on = true;
    char sendBuf[DEFAULT_BUFLEN];
    std::string ls;

    // Receive until the peer shuts down the connection
    do {
        // Clears the buffer
        memset(recvbuf, 0, sizeof(recvbuf));

        // Checks for available messages
        do {
            iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);

            // Bytes received
            printf("%s\n", recvbuf);
            fflush(stdout);

            if (iResult > 0) {
                // Build shell functionality
                recvbuf[iResult] = '\0';

                // Print working directory
                if (strcmp(recvbuf, "pwd") == 0) {
                    cwd = std::filesystem::current_path().string();

                    printf("Current directory on server: %s\n", cwd.c_str()); // Log current directory

                    iSendResult = send(ClientSocket, cwd.c_str(), (int) cwd.length() + 1, 0);
                    if (iSendResult == SOCKET_ERROR)
                        goto send_cleanup_error;

                    printf("Sent current directory to client\n"); // Log that directory was sent
                }

                // Exit the loop
                if(strcmp(recvbuf, "exit") == 0) {
                    on = false;
                    printf("Closing connection...\n");
                    break;
                }

                // Change working directory
                if (strncmp(recvbuf, "cd ", 3) == 0) {
                    // Remove the "cd "
                    if (shiftStrLeft(recvbuf, 3) != 0) {
                        fprintf(stderr, "Could not shift recvbuf\n");
                        closesocket(ClientSocket);
                        WSACleanup();
                        return 1;
                    }

                    try {
                        printf("Current directory on server: %s\n", cwd.c_str()); // Log current directory
                        std::filesystem::current_path(recvbuf); // Change the directory
                        std::string cwd = std::filesystem::current_path().string(); // Get the current directory as string

                        int n = snprintf(sendBuf, DEFAULT_BUFLEN, "Changed working directory to %s", cwd.c_str());
                        if (n >= DEFAULT_BUFLEN) {
                            fprintf(stderr, "sendBuf is too small for the message\n");
                            closesocket(ClientSocket);
                            WSACleanup();
                            return 1;
                        }

                        iSendResult = send(ClientSocket, sendBuf, n, 0); // Send the response
                        printf("Sent command to client: %s", sendBuf); // Log that you sent a command
                        if (iSendResult == SOCKET_ERROR) {
                            fprintf(stderr, "send failed with error: %d\n", WSAGetLastError());
                            closesocket(ClientSocket);
                            WSACleanup();
                            return 1;
                        }
                    } catch (const std::filesystem::filesystem_error& e) {
                        std::cerr << "Error: " << e.what() << std::endl;

                        // Send error message to client
                        snprintf(sendBuf, DEFAULT_BUFLEN, "Error changing directory: %s", e.what());
                        send(ClientSocket, sendBuf, (int) strlen(sendBuf), 0); // Send the error message
                    }
                }

                if (strncmp(recvbuf, "ls", 2) == 0) {
                    // Prints the ls of the cwd into a tmp file
                    if (strcmp(recvbuf, "ls") == 0) {
                        if (run_ls(std::filesystem::current_path().string()) != 0) {
                            fprintf(stderr, "could not execute ls %s\n", recvbuf);
                            closesocket(ClientSocket);
                            WSACleanup();
                            return 1;
                        }
                    } else {
                        // Removes the "ls "
                        if (shiftStrLeft(recvbuf, 3) != 0) {
                            fprintf(stderr, "could not shift recvbuf\n");
                            closesocket(ClientSocket);
                            WSACleanup();
                            return 1;
                        }

                        // Runs the command, printing the ls command for the given path into a tmp file
                        if (run_ls(recvbuf) != 0) {
                            fprintf(stderr, "could not execute ls %s\n", recvbuf);
                            closesocket(ClientSocket);
                            WSACleanup();
                            return 1;
                        }
                    }

                    // Opens the file
                    std::ifstream tmpFile(R"(..\..\Host\src\scripts\out.txt)");
                    if (!tmpFile.is_open()) {
                        fprintf(stderr, "could not open out.txt\n");
                        closesocket(ClientSocket);
                        WSACleanup();
                        return 1;
                    }

                    // Turns contents into a string of everything written in the tmp file
                    std::string contents;
                    std::string line;
                    while (std::getline(tmpFile, line)) {
                        contents.append(line + "\n");
                    }
                    tmpFile.close();
                    remove(R"(..\..\Host\src\scripts\out.txt)");

                    iSendResult = send(ClientSocket, contents.c_str(), (int)contents.length(), 0);
                    if (iSendResult == SOCKET_ERROR) {
                        goto send_cleanup_error;
                    }
                }

                // Send back that the sent command doesn't exist
                else
                {
                    sprintf(sendBuf, "The command doesn't exist");
                    iSendResult = send(ClientSocket, sendBuf, (int) strlen(sendBuf) + 1, 0);
                    if (iSendResult == SOCKET_ERROR) {
                        iResult = shutdown(ClientSocket, SD_SEND);
                        if(iResult == SOCKET_ERROR) {
                            fprintf(stderr, "shutdown failed with error: %d\n", WSAGetLastError());
                            closesocket(ClientSocket);
                            WSACleanup();
                            return 1;
                        }
                    }
                    goto send_cleanup_error;
                }
            }

            else if(iResult < 0) {
                fprintf(stderr, "recv failed with error: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }
        } while (iResult > 0);
    } while (on);

    // Shutdown the connection
    iResult = shutdown(ClientSocket, SD_SEND);
    if(iResult == SOCKET_ERROR) {
        fprintf(stderr, "shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    // Cleanup
    closesocket(ClientSocket);
    WSACleanup();

    return 0;

send_cleanup_error:
    fprintf(stderr, "send failed with error: %d, tried to send: %d bytes\n", WSAGetLastError(), (int) cwd.length() + 1);
    closesocket(ClientSocket);
    WSACleanup();
    return 1;
}

int shiftStrLeft(char *str, int num) {
    size_t len = strlen(str);

    if(len > 0 && num < len) {
        memmove(str, str + num, len - num + 1);
        return 0;
    }
    else
        return 1;
}