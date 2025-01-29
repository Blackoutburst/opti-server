#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils/ioUtils.h"
#include "utils/string.h"
#include "utils/mutex.h"
#include "utils/thread.h"
#include "network/server.h"

static U8 running = 0;
static U32 clientId = 0;

static TCP_CLIENT** tcpClients = NULL;

#if defined(_WIN32) || defined(_WIN64)
    static CRITICAL_SECTION mutex;
    static SOCKET serverSocket = NULL;
#else
    static pthread_mutex_t mutex;
    static I32 serverSocket = 0;
#endif

/// CLIENTS ///
#if defined(_WIN32) || defined(_WIN64)
void addClient(SOCKET socket, HANDLE thread) {
    mutexLock(&mutex);

    U8* name = encodeString((const U8*)"Guest", 64);
    TCP_CLIENT* client = malloc(sizeof(TCP_CLIENT*));
    client->id = clientId;
    cliclent->socket = socket;
    client->thread = thread;
    client->position.x = 0;
    client->position.y = 0;
    client->position.z = 0;c
    client->renderDistance = 2;
    memcpy(client->name, name, 64);
    free(name);

    for (U32 i = 0; i < MAX_TCP_CLIENT; i++) {
        if (tcpClients[i] != NULL) continue;
        tcpClients[i] = client;
        break;
    }

    clientId++;

    mutexUnlock(&mutex);
}
#else
void addClient(I32 socket, pthread_t thread) {
    mutexLock(&mutex);

    U8* name = encodeString((const U8*)"Guest", 64);
    TCP_CLIENT* client = malloc(sizeof(TCP_CLIENT*));
    client->id = clientId;
    client->socket = socket;
    client->thread = thread;
    client->position.x = 0;
    client->position.y = 0;
    client->position.z = 0;
    client->renderDistance = 2;
    memcpy(client->name, name, 64);
    free(name);

    for (U32 i = 0; i < MAX_TCP_CLIENT; i++) {
        if (tcpClients[i] != NULL) continue;
        tcpClients[i] = client;
        break;
    }

    clientId++;

    mutexUnlock(&mutex);
}
#endif

void removeClient(U32 id) {
    mutexLock(&mutex);

    for (U32 i = 0; i < MAX_TCP_CLIENT; i++) {
        if (tcpClients[i] == NULL) continue;
        if (tcpClients[i]->id == id) {
            free(tcpClients[i]);
            tcpClients[i] = NULL;
        }
    }

    mutexUnlock(&mutex);
}

/// READ ///
#if defined(_WIN32) || defined(_WIN64)
     DWORD WINAPI serverRead(LPVOID arg) {
        SOCKET clientSocket = *(SOCKET*)arg;

        while (running && clientSocket != INVALID_SOCKET) {
            //recv(clientSocket, buffer, size, 0);
        }

        return 0;
     }
#else
    void* serverRead(void* arg) {
        I32 clientSocket = *(I32*)arg;

        while (running && clientSocket >= 0) {
            //recv(clientSocket, buffer, size, 0);
        }

        return NULL;
    }
#endif

/// WRITE ///
#if defined(_WIN32) || defined(_WIN64)
    void serverWriteWIN(U8* buffer, U32 size) {
        for (U32 i = 0; i < MAX_TCP_CLIENT; i++) {
            if (tcpClients[i] == NULL) continue;
            send(tcpClients[i]->socket, buffer, size, 0);
        }
    }
#else
    void serverWritePOSIX(U8* buffer, U32 size) {
        for (U32 i = 0; i < MAX_TCP_CLIENT; i++) {
            if (tcpClients[i] == NULL) continue;
            send(tcpClients[i]->socket, buffer, size, 0);
        }
    }
#endif

void serverWrite(U8* buffer, U32 size) {
    mutexLock(&mutex);
        #if defined(_WIN32) || defined(_WIN64)
            serverWriteWIN(buffer, size);
        #else
            serverWritePOSIX(buffer, size);
        #endif
    mutexUnlock(&mutex);
}

/// ACCEPT ///
#if defined(_WIN32) || defined(_WIN64)
    void serverAcceptWIN(void) {
        struct sockaddr_in clientAddress;
        socklen_t clientLength;
        SOCKET clientSocket = NULL;

        clientLength = sizeof(clientAddress);
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientLength);
        if (clientSocket == INVALID_SOCKET) {
            println("Client accept failed");
        }

        HANDLE thread = createThread(serverRead, &clientSocket);
        addClient(clientSocket, thread);
    }
#else
    void serverAcceptPOSIX(void) {
        struct sockaddr_in clientAddress;
        socklen_t clientLength;
        I32 clientSocket = 0;

        clientLength = sizeof(clientAddress);
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientLength);
        if (clientSocket < 0) {
            println("Client accept failed");
        }

        pthread_t thread = createThread(serverRead, &clientSocket);
        addClient(clientSocket, thread);
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

        clfor (U32 i = 0; i < MAX_TCP_CLIENT; i++) {
            if (tcpClients[i] == NULL) continue;
            close(tcpClients[i]->socket);
            joinThread(tcpClients[i]->thread);
        }
    }
#else
    void serverCleanPOSIX(void) {
        running = 0;
        close(serverSocket);
        serverSocket = 0;

        for (U32 i = 0; i < MAX_TCP_CLIENT; i++) {
            if (tcpClients[i] == NULL) continue;
            close(tcpClients[i]->socket);
            joinThread(tcpClients[i]->thread);
        }
    }
#endif

void serverClean(void) {
    mutexDestroy(&mutex);

    #if defined(_WIN32) || defined(_WIN64)
        serverCleanWIN();
    #else
        serverCleanPOSIX();
    #endif

    for (U32 i = 0; i < MAX_TCP_CLIENT; i++) {
        if (tcpClients[i] == NULL) continue;
        free(tcpClients[i]);
        tcpClients[i] = NULL;
    }
    free(tcpClients);
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
    mutexCreate(&mutex);

    tcpClients = malloc(sizeof(TCP_CLIENT*) * MAX_TCP_CLIENT);
    for (U32 i = 0; i < MAX_TCP_CLIENT; i++) tcpClients[i] = NULL;

    #if defined(_WIN32) || defined(_WIN64)
        serverInitWIN();
    #else
        serverInitPOSIX();
    #endif
}
