#define WIN32_LEAN_AND_MEAN

/*
ls - List all files and directories in the current directory. //
cd - Change current directory. //
pwd - Print working directory. //
cat - Concatenate and display the content of files.
echo - Output the inputs.
mkdir - Create a new directory. //
rmdir - Removes a directory. //
rm - Remove files or empty directories. //
touch - Create an empty file. //
mv - Move or rename files or directories.
cp - Copy files or directories.
find - Search for files in a directory hierarchy.
grep - Search text using patterns.
exit - Exit the shell. //
copy_pc - copy's a file to the client pc
*/

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

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

// Basic client request operations
int handlePwdCommand(SOCKET clientSocket);
void handleExitCommand(SOCKET clientSocket);
int handleChangeDirectoryCommand(SOCKET clientSocket, const char* path);
int handleLsCommand(SOCKET clientSocket, char *command);
void sendCmdDoesntExist(SOCKET clientSocket);
int handleMakeDirectoryCommand(SOCKET socket, char *path);
int handleTouchFileCommand(SOCKET clientSocket, char *fileName);
int handleRemoveDirectoryCommand(SOCKET socket, char *path);
int handleRemoveFileCommand(SOCKET clientSocket, char *fileName);

// Error function
void handleError(SOCKET clientSocket, const char *command);

// Misc functions
int shiftStrLeft(char *str, int num);

int __cdecl main()
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

    do {
        memset(recvbuf, 0, sizeof(recvbuf));
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);

        if (iResult > 0) {
            recvbuf[iResult] = '\0';

            if (strcmp(recvbuf, "pwd") == 0) {
                printf("%s\n", recvbuf);
                if (handlePwdCommand(ClientSocket) == -1) {
                    handleError(ClientSocket, "pwd");
                    return 1;
                }
            }
            else if(strcmp(recvbuf, "exit") == 0) {
                printf("%s\n", recvbuf);
                handleExitCommand(ClientSocket);
                return 0;
            }
            else if (strncmp(recvbuf, "cd ", 3) == 0) {
                printf("%s\n", recvbuf);
                if (handleChangeDirectoryCommand(ClientSocket, recvbuf + 3) == -1) {
                    handleError(ClientSocket, "cd");
                    return 1;
                }
            }
            else if (strncmp(recvbuf, "ls", 2) == 0) {
                printf("%s\n", recvbuf);
                if (handleLsCommand(ClientSocket, recvbuf) == -1) {
                    handleError(ClientSocket, "ls");
                    return 1;
                }
            }
            else if(strncmp(recvbuf, "mkdir ", 6) == 0) {
                printf("%s\n", recvbuf);
                if(handleMakeDirectoryCommand(ClientSocket, recvbuf) == -1) {
                    handleError(ClientSocket, "mkdir");
                    return 1;
                }
            }
            else if(strncmp(recvbuf, "rmdir ", 6) == 0) {
                printf("%s\n", recvbuf);
                if(handleRemoveDirectoryCommand(ClientSocket, recvbuf) == -1) {
                    handleError(ClientSocket, "rmdir");
                    return 1;
                }
            }
            else if(strncmp(recvbuf, "touch ", 6) == 0) {
                printf("%s\n", recvbuf);
                if(handleTouchFileCommand(ClientSocket, recvbuf) == -1) {
                    handleError(ClientSocket, "touch");
                    return 1;
                }
            }
            else if(strncmp(recvbuf, "rm ", 3) == 0) {
                printf("%s\n", recvbuf);
                if(handleRemoveFileCommand(ClientSocket, recvbuf) == -1) {
                    handleError(ClientSocket, "rm");
                    return 1;
                }
            }


            else {
                sendCmdDoesntExist(ClientSocket);
            }
        }
        else if(iResult < 0) {
            fprintf(stderr, "recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }
    } while (iResult > 0);

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

int handleRemoveFileCommand(SOCKET clientSocket, char *fileName) {
    shiftStrLeft(fileName, 3);

    // Removes specified file
    try {
        if(std::filesystem::remove(fileName)) {
            std::string sendSuc = std::format("{} was successfully removed!\f", fileName);

            int iSendResult = send(clientSocket, sendSuc.c_str(), (int) sendSuc.length(), 0);
            if(iSendResult == SOCKET_ERROR) {
                fprintf(stderr, "send failed with error: %d\n", WSAGetLastError());
                return -1;
            }
        } else {
            return -1;
        }

    } catch (const std::filesystem::filesystem_error& e) {
        fprintf(stderr, "Error removing file: %s\n", e.what());
        return -1;
    }
    return 1;
}

int handleTouchFileCommand(SOCKET clientSocket, char *fileName) {
    shiftStrLeft(fileName, 6);

    std::ofstream file(fileName);
    if(!file)
    {
        std::cerr << "Error in opening " << fileName << std::endl;
        return -1;
    }

    std::string sendSuc = std::format("{} was successfully created!\f", fileName);

    int iSendResult = send(clientSocket, sendSuc.c_str(), (int) sendSuc.length(), 0);
    if(iSendResult == SOCKET_ERROR) {
        fprintf(stderr, "send failed with error: %d\n", WSAGetLastError());
        return -1;
    }

    file.close();
    return 0;
}

int handleRemoveDirectoryCommand(SOCKET clientSocket, char *path) {
    shiftStrLeft(path, 6);

    // Removes folder + all files inside of it recursively
    try {
        std::filesystem::remove_all(path);
        std::string sendSuc = std::format("Directory {} was successfully removed!\f", path);

        int iSendResult = send(clientSocket, sendSuc.c_str(), (int) sendSuc.length(), 0);
        if(iSendResult == SOCKET_ERROR) {
            fprintf(stderr, "send failed with error: %d\n", WSAGetLastError());
            return -1;
        }

    } catch (const std::error_code& e) {
        fprintf(stderr, "Error removing directory: %s\n", e.message().c_str());
        return -1;
    }
    return 1;
}

int handleMakeDirectoryCommand(SOCKET clientSocket, char *path) {
    shiftStrLeft(path, 6);

    // Creates a directory if it doesn't exist already
    if(CreateDirectory(path, NULL) || ERROR_ALREADY_EXISTS == GetLastError())
    {
        std::string sendSuc = std::format("Directory {} was successfully created!\f", path);
        int iSendResult = send(clientSocket, sendSuc.c_str(), (int) sendSuc.length(), 0);
        if(iSendResult == SOCKET_ERROR) {
            fprintf(stderr, "send failed with error: %d\n", WSAGetLastError());
            return -1;
        }
        return 0;
    }
    else
        return -1;
}

int handlePwdCommand(SOCKET clientSocket) {
    std::string cwd = std::filesystem::current_path().string();
    printf("Current directory on server: %s\n", cwd.c_str());
    cwd += '\f';

    int iSendResult = send(clientSocket, cwd.c_str(), (int) cwd.length(), 0);
    if (iSendResult == SOCKET_ERROR) {
        fprintf(stderr, "send failed with error: %d\n", WSAGetLastError());
        return -1;
    }

    return 0;
}

void handleExitCommand(SOCKET ClientSocket) {
    printf("Closing connection...\n");
}

int handleChangeDirectoryCommand(SOCKET clientSocket, const char* path) {
    try {
        std::filesystem::current_path(path);
        std::string cwd = std::filesystem::current_path().string();

        char sendBuf[DEFAULT_BUFLEN];
        int n = snprintf(sendBuf, DEFAULT_BUFLEN, "Changed working directory to %s\f", cwd.c_str());

        if (n >= DEFAULT_BUFLEN) {
            fprintf(stderr, "sendBuf is too small for the message\n");
            return -1;
        }

        int iSendResult = send(clientSocket, sendBuf, n, 0);
        if (iSendResult == SOCKET_ERROR) {
            fprintf(stderr, "send failed with error: %d\n", WSAGetLastError());
            return -1;
        }

        return 0;
    }
    catch (const std::filesystem::filesystem_error& e) {
        char sendBuf[DEFAULT_BUFLEN];
        snprintf(sendBuf, DEFAULT_BUFLEN, "Error changing directory: %s\n", e.what());

        int iSendResult = send(clientSocket, sendBuf, (int) strlen(sendBuf), 0);
        if(iSendResult == SOCKET_ERROR) {
            fprintf(stderr, "send failed with error: %d\n", WSAGetLastError());
            return -1;
        }

        return 0;
    }
}

int handleLsCommand(SOCKET clientSocket, char *command) {
    std::string prevDirectory;
    std::string cwd;
    bool thisDirectory = false;

    if(strcmp(command, "ls") == 0)
        cwd = std::filesystem::current_path().string();
    else {
        try{
            shiftStrLeft(command, 3);
            prevDirectory = std::filesystem::current_path().string();
            std::filesystem::current_path(command);
            cwd = std::filesystem::current_path().string();
            thisDirectory = true;
        } catch(const std::exception& e) {
            std::cerr << "Error in handleLS not current directory, error code: " << e.what() << std::endl;
            return -1;
        }
    }
    try {
        std::string directoryContents = "Directory listing for " + cwd + "\n";

        // directory_iterator is used to traverse all the entries of directory
        for (const auto & entry : std::filesystem::directory_iterator(cwd)) {
            directoryContents += entry.path().filename().string() + "\n";
        }

        directoryContents += '\f';
        int iSendResult = send(clientSocket, directoryContents.c_str(), (int) directoryContents.length(), 0);

        if (iSendResult == SOCKET_ERROR) {
            fprintf(stderr, "send failed with error: %d\n", WSAGetLastError());
            return -1;
        }

        if(thisDirectory)
            std::filesystem::current_path(prevDirectory);

        return 0;

    } catch (const std::exception& e) {
        char sendBuf[DEFAULT_BUFLEN];
        snprintf(sendBuf, DEFAULT_BUFLEN, "Error executing ls: %s\f", e.what());

        int iSendResult = send(clientSocket, sendBuf, strlen(sendBuf), 0);
        if(iSendResult == SOCKET_ERROR) {
            fprintf(stderr, "send failed with error: %d\n", WSAGetLastError());
            return -1;
        }

        return 0;
    }
}

void sendCmdDoesntExist(SOCKET ClientSocket) {
    char sendBuf[DEFAULT_BUFLEN];
    sprintf(sendBuf, "The command doesn't exist\f");
    int iSendResult = send(ClientSocket, sendBuf, (int) strlen(sendBuf) + 1, 0);
    if (iSendResult == SOCKET_ERROR) {
        fprintf(stderr, "send failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
    }
}

void handleError(SOCKET clientSocket, const char *command) {
    std::string message = std::format("Error in performing {} with error code: {}", command, WSAGetLastError());
    std::cerr << message << std::endl;

    message += '\f';
    int iSendResult = send(clientSocket, message.c_str(), (int) message.length(), 0);
    if(iSendResult == SOCKET_ERROR)
        fprintf(stderr, "send failed with error: %d\n", WSAGetLastError());

    closesocket(clientSocket);
    WSACleanup();
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