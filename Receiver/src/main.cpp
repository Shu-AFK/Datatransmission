// TODO: Unable to perform commands

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <stdio.h>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

int __cdecl main(int argc, char **argv)
{
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
            *ptr = NULL,
            hints;
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;

    // Validate the parameters
    if (argc != 2) {
        printf("usage: %s server-name\n", argv[0]);
        return 1;
    }

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
                               ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    char buff[1024];
    bool on = true;

    do {
        // Clear the buffer
        memset(buff, 0, sizeof(buff));
        memset(recvbuf, 0, sizeof(recvbuf));

        std::cout << "shell $ ";
        std::cin.getline(buff, sizeof(buff));

        if(strcmp(buff, "exit") == 0) {
            on = false;
            iResult = send(ConnectSocket, buff, (int)strlen(buff) + 1, 0); // +1 for null terminator
            if(iResult == SOCKET_ERROR)
            {
                fprintf(stderr, "send failed with error: %d\n", WSAGetLastError());
                fprintf(stderr, "could not perform %s\n", buff);
                closesocket(ConnectSocket);
                WSACleanup();
                return 1;
            }
        }

        if(strcmp(buff, "pwd") == 0) {
            iResult = send(ConnectSocket, buff, (int)strlen(buff), 0);
            if(iResult == SOCKET_ERROR)
            {
                fprintf(stderr, "send failed with error: %d\n", WSAGetLastError());
                fprintf(stderr, "could not perform %s\n", buff);
                closesocket(ConnectSocket);
                WSACleanup();
                return 1;
            }

            iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
            if(iResult > 0) {
                recvbuf[iResult] = '\0';
                printf("%s\n", recvbuf);
            }

            else if(iResult < 0)
            {
                fprintf(stderr, "recv failed with error: %d\n", WSAGetLastError());
                closesocket(ConnectSocket);
                WSACleanup();
                return 1;
            }
        }
    } while (on);

    // shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}