/* SOURCE: https://learn.microsoft.com/en-us/windows/win32/winsock/complete-server-code
 * READ: https://learn.microsoft.com/en-us/training/modules/build-a-tcp-echo-client/?source=recommendations */

#undef UNICODE

#define WIN32_LEAN_AND_MEAN

// Needed Libs
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <fstream>
#include "helper.h"
#include <unistd.h>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

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

    char cwd[1024];
    bool on = true;

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
                if (strcmp(recvbuf, "pwd") == 0) {
                    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
                        iSendResult = send(ClientSocket, cwd, strlen(cwd) + 1, 0);
                        if (iSendResult == SOCKET_ERROR) {
                            fprintf(stderr, "send failed with error: %d\n", WSAGetLastError());
                            closesocket(ClientSocket);
                            WSACleanup();
                            return 1;
                        }
                    }
                }
                if(strcmp(recvbuf, "exit") == 0) {
                    on = false;
                    printf("Closing connection...\n");
                    break;
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
}