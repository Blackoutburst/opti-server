#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils/ioUtils.h"
#include "utils/thread.h"
#include "network/server.h"

static U8 running = 0;

#if defined(_WIN32) || defined(_WIN64)
    static SOCKET serverSocket = NULL;
#else
    static I32 serverSocket = 0;
#endif

/// READ ///
#if defined(_WIN32) || defined(_WIN64)
     DWORD WINAPI serverRead(LPVOID arg) {
        SOCKET clientSocket = *(SOCKET*)arg;
        free(arg);

        while (running && clientSocket != INVALID_SOCKET) {
            //recv(clientSocket, buffer, size, 0);
        }

        return 0;
     }
#else
    void* serverRead(void* arg) {
        I32 clientSocket = *(I32*)arg;
        free(arg);

        while (running && clientSocket >= 0) {
            //recv(clientSocket, buffer, size, 0);
        }

        return NULL;
    }
#endif

/// WRITE ///
#if defined(_WIN32) || defined(_WIN64)
    void serverWriteWIN(U8* buffer) {

    }
#else
    void serverWritePOSIX(U8* buffer) {

    }
#endif
void serverWrite(U8* buffer) {
    #if defined(_WIN32) || defined(_WIN64)
        serverWriteWIN(buffer);
    #else
        serverWritePOSIX(buffer);
    #endif
}

/// ACCEPT ///
#if defined(_WIN32) || defined(_WIN64)
    void serverAcceptWIN(void) {
        struct sockaddr_in clientAddress;
        socklen_t clientLength;
        SOCKET* clientSocket = malloc(sizeof(SOCKET));

        clientLength = sizeof(clientAddress);
        *clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientLength);
        if (*clientSocket == INVALID_SOCKET) {
            println("Client accept failed");
        }

        createThread(serverRead, clientSocket);
    }
#else
    void serverAcceptPOSIX(void) {
        struct sockaddr_in clientAddress;
        socklen_t clientLength;
        I32* clientSocket = malloc(sizeof(I32));

        clientLength = sizeof(clientAddress);
        *clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientLength);
        if (*clientSocket < 0) {
            println("Client accept failed");
        }

        createThread(serverRead, clientSocket);
    }
#endif

void serverAccept(void) {
    #if defined(_WIN32) || defined(_WIN64)
        serverAcceptWIN();
    #else
        serverAcceptPOSIX();
    #endif
}

/// LISTEN ///
#if defined(_WIN32) || defined(_WIN64)
    void serverListenWIN(void) {
        while (running) {
            if (listen(server_fd, TCP_LISTENT_QUEUE_SIZE) == SOCKET_ERROR) {
                println("Server listen failed");
                serverClean();
            }

            serverAccept();
        }
    }
#else
    void serverListenPOSIX(void) {
        while (running) {
            if (listen(serverSocket, TCP_LISTENT_QUEUE_SIZE) < 0) {
                println("Server listen failed");
                serverClean();
            }

            serverAccept();
        }
    }
#endif

void serverListen(void) {
    running = 1;
    printf("Server listening on port [TCP:%i]\n", TCP_PORT);

    #if defined(_WIN32) || defined(_WIN64)
        serverListenWIN();
    #else
        serverListenPOSIX();
    #endif
}

/// CLEAN ///
#if defined(_WIN32) || defined(_WIN64)
    void serverCleanWIN(void) {
        running = 0;
        closesocket(serverSocket);
        WSACleanup();
        serverSocket = NULL;
    }
#else
    void serverCleanPOSIX(void) {
        running = 0;
        close(serverSocket);
        serverSocket = 0;
    }
#endif

void serverClean(void) {
    #if defined(_WIN32) || defined(_WIN64)
        serverCleanWIN();
    #else
        serverCleanPOSIX();
    #endif
}
/// INIT ///

#if defined(_WIN32) || defined(_WIN64)
    void serverInitWIN(void) {
        struct sockaddr_in serverAddress;
        WSADATA wsaData;

        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            println("WSAStartup failed")
            exit(1);
        }

        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == INVALID_SOCKET) {
            println("Server socket creation failed");
            WSACleanup();
            exit(1);
        }

        memset(&serverAddress, 0, sizeof(serverAddress));
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = INADDR_ANY;
        serverAddress.sin_port = htons(TCP_PORT);

        if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
            println("Server bind failed");
            serverClean();
        }

        serverListen();
    }
#else
    void serverInitPOSIX(void) {
        struct sockaddr_in serverAddress;

        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket < 0) {
            println("Server socket creation failed");
            exit(1);
        }

        memset(&serverAddress, 0, sizeof(serverAddress));
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = INADDR_ANY;
        serverAddress.sin_port = htons(TCP_PORT);

        if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
            println("Server bind failed");
            serverClean();
        }

        serverListen();
    }
#endif

void serverInit(void) {
    #if defined(_WIN32) || defined(_WIN64)
        serverInitWIN();
    #else
        serverInitPOSIX();
    #endif
}
