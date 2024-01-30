#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <stdio.h>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

int add_backslashes(char *str);

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

    std::string buff;
    bool on = true;

    do {
        // Clear the buffer
        buff.clear();
        memset(recvbuf, 0, sizeof(recvbuf));

        std::cout << "shell $ ";
        std::getline(std::cin, buff);

        if(strcmp(buff.c_str(), "exit") == 0) {
            on = false;
            iResult = send(ConnectSocket, buff.c_str(), (int) buff.length() + 1, 0); // +1 for null terminator
            printf("Sent command to server: %s\n", buff.c_str()); // Log that you sent a command
            if(iResult == SOCKET_ERROR)
            {
                fprintf(stderr, "send failed with error: %d\n", WSAGetLastError());
                fprintf(stderr, "could not perform %s\n", buff.c_str());
                closesocket(ConnectSocket);
                WSACleanup();
                return 1;
            }
        }

        if(strcmp(buff.c_str(), "pwd") == 0) {
            iResult = send(ConnectSocket, buff.c_str(), (int) buff.length(), 0);
            printf("Sent command to server: %s\n", buff.c_str()); // Log that you sent a command
            if(iResult == SOCKET_ERROR)
                goto cleanup_send_error;

            iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
            printf("Received response from server: %s\n", recvbuf); // Log that you received a response
            if(iResult > 0) {
                recvbuf[iResult] = '\0';
                printf("%s\n", recvbuf);
            }

            else if(iResult < 0)
                goto cleanup_recv_error;
        }

        if(strncmp(buff.c_str(), "cd ", 3) == 0)
        {
            iResult = send(ConnectSocket, buff.c_str(), (int) buff.length(), 0);
            printf("Sent command to server: %s\n", buff.c_str()); // Log that you sent a command
            if(iResult == SOCKET_ERROR)
                goto cleanup_send_error;

            iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
            printf("Received response from server: %s\n", recvbuf); // Log that you received a response
            if(iResult > 0) {
                recvbuf[iResult] = '\0';
                printf("%s\n", recvbuf);
            }

            else if(iResult < 0)
                goto cleanup_recv_error;
        }

        if(strncmp(buff.c_str(), "ls", 2) == 0)
        {
            iResult = send(ConnectSocket, buff.c_str(), (int) buff.length(), 0);
            printf("Sent command to server: %s\n", buff.c_str()); // Log that you sent a command
            if(iResult == SOCKET_ERROR)
                goto cleanup_send_error;

            iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
            printf("Received response from server: %s\n", recvbuf); // Log that you received a response
            if(iResult > 0) {
                recvbuf[iResult] = '\0';
                printf("%s\n", recvbuf);
            }

            else if(iResult < 0)
                goto cleanup_recv_error;
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

cleanup_send_error:
    fprintf(stderr, "send failed with error: %d\n", WSAGetLastError());
    closesocket(ConnectSocket);
    WSACleanup();
    return 1;

cleanup_recv_error:
    fprintf(stderr, "recv failed with error: %d\n", WSAGetLastError());
    closesocket(ConnectSocket);
    WSACleanup();
    return 1;
}