#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils/math.h"
#include "utils/ioUtils.h"
#include "utils/string.h"
#include "utils/mutex.h"
#include "utils/thread.h"
#include "network/server.h"
#include "network/client.h"
#include "network/encoder.h"
#include "network/packet.h"

static U8 status = 0;

static U8 running = 0;
static U32 clientId = 0;

static TCP_CLIENT** tcpClients = NULL;

#if defined(_WIN32) || defined(_WIN64)
    static CRITICAL_SECTION mutex;
    static SOCKET serverSocket = 0;
#else
    static pthread_mutex_t mutex;
    static I32 serverSocket = 0;
#endif

U32 getClientId(void) {
    return clientId;
}

/// CLIENTS ///
void _serverSendAddEntity(TCP_CLIENT* client, TCP_CLIENT* entity) {
    C01ADD_ENTITY packet;
    packet.id = CLIENT_PACKET_ADD_ENTITY;
    packet.entityId = entity->id;
    packet.x = entity->position.x;
    packet.y = entity->position.y;
    packet.z = entity->position.z;
    packet.yaw = entity->yaw;
    packet.pitch = entity->pitch;
    memcpy(packet.name, entity->name, 64);

    U8* buffer = encodePacketAddEntity(&packet);
    _serverWrite(client, buffer, getClientPacketSize(CLIENT_PACKET_ADD_ENTITY)); 

    free(buffer);
}

void _serverSendRemoveEntity(TCP_CLIENT* client, U32 entityId) {
    C02REMOVE_ENTITY packet;
    packet.id = CLIENT_PACKET_REMOVE_ENTITY;
    packet.entityId = entityId;

    U8* buffer = encodePacketRemoveEntity(&packet);
    _serverWrite(client, buffer, getClientPacketSize(CLIENT_PACKET_REMOVE_ENTITY)); 

    free(buffer);
}

#if defined(_WIN32) || defined(_WIN64)
    TCP_CLIENT* addClient(SOCKET socket) {
        mutexLock(&mutex);

        U8* name = encodeString((const U8*)"Guest", 64);
        TCP_CLIENT* client = malloc(sizeof(TCP_CLIENT));
        client->id = clientId;
        client->socket = socket;
        client->position.x = 0;
        client->position.y = 0;
        client->position.z = 0;
        client->yaw = 0;
        client->pitch = 0;
        client->renderDistance = 2;
        client->chunks = malloc(sizeof(CHUNK_HASHMAP) * CUBE(client->renderDistance * 2));
        for (U32 i = 0; i < CUBE(client->renderDistance * 2); i++) {
            client->chunks[i].position.x = 0;
            client->chunks[i].position.y = 0;
            client->chunks[i].position.z = 0;
            client->chunks[i].used = 0;
        }

        memcpy(client->name, name, 64);
        free(name);

        for (U32 i = 0; i < MAX_TCP_CLIENT; i++) {
            if (tcpClients[i] == NULL) continue;
            _serverSendAddEntity(tcpClients[i], client);
            _serverSendAddEntity(client, tcpClients[i]);
        }

        for (U32 i = 0; i < MAX_TCP_CLIENT; i++) {
            if (tcpClients[i] != NULL) continue;
            tcpClients[i] = client;
            break;
        }

        printf("New client %i\n", client->id);

        clientId++;

        mutexUnlock(&mutex);
        return client;
    }

    TCP_CLIENT* getClientBysocket(SOCKET socket) {
        mutexLock(&mutex);

        for (U32 i = 0; i < MAX_TCP_CLIENT; i++) {
            if (tcpClients[i] == NULL) continue;
            if (tcpClients[i]->socket == socket) {
                mutexUnlock(&mutex);
                return tcpClients[i];
            }
        }

        mutexUnlock(&mutex);

        return NULL;
    }
#else
    TCP_CLIENT* addClient(I32 socket) {
        mutexLock(&mutex);

        U8* name = encodeString((const U8*)"Guest", 64);
        TCP_CLIENT* client = malloc(sizeof(TCP_CLIENT));
        client->id = clientId;
        client->socket = socket;
        client->position.x = 0;
        client->position.y = 0;
        client->position.z = 0;
        client->yaw = 0;
        client->pitch = 0;
        client->renderDistance = 2;
        client->chunks = malloc(sizeof(CHUNK_HASHMAP) * CUBE(client->renderDistance * 2));
        for (U32 i = 0; i < CUBE(client->renderDistance * 2); i++) {
            client->chunks[i].position.x = 0;
            client->chunks[i].position.y = 0;
            client->chunks[i].position.z = 0;
            client->chunks[i].used = 0;
        }
        memcpy(client->name, name, 64);
        free(name);

        for (U32 i = 0; i < MAX_TCP_CLIENT; i++) {
            if (tcpClients[i] == NULL) continue;
            _serverSendAddEntity(tcpClients[i], client);
            _serverSendAddEntity(client, tcpClients[i]);
        }

        for (U32 i = 0; i < MAX_TCP_CLIENT; i++) {
            if (tcpClients[i] != NULL) continue;
            tcpClients[i] = client;
            break;
        }

        printf("New client %i\n", client->id);

        clientId++;

        mutexUnlock(&mutex);
        return client;
    }

    TCP_CLIENT* getClientBysocket(I32 socket) {
        mutexLock(&mutex);

        for (U32 i = 0; i < MAX_TCP_CLIENT; i++) {
            if (tcpClients[i] == NULL) continue;
            if (tcpClients[i]->socket == socket) {
                mutexUnlock(&mutex);
                return tcpClients[i];
            }
        }

        mutexUnlock(&mutex);

        return NULL;
    }
#endif

TCP_CLIENT** getAllClients(void) {
    return tcpClients;
}

void removeClient(U32 id) {
    mutexLock(&mutex);

    for (U32 i = 0; i < MAX_TCP_CLIENT; i++) {
        if (tcpClients[i] == NULL) continue;
        if (tcpClients[i]->id == id) continue;
        _serverSendRemoveEntity(tcpClients[i], id);
    }

    for (U32 i = 0; i < MAX_TCP_CLIENT; i++) {
        if (tcpClients[i] == NULL) continue;
        if (tcpClients[i]->id == id) {
            #if defined(_WIN32) || defined(_WIN64)
                closesocket(tcpClients[i]->socket);
                tcpClients[i]->socket = INVALID_SOCKET;
            #else
                close(tcpClients[i]->socket);
                tcpClients[i]->socket = -1;
            #endif
            free(tcpClients[i]->chunks);
            free(tcpClients[i]);
            tcpClients[i] = NULL;
            break;
        }
    }

    printf("Client %i removed\n", id);

    mutexUnlock(&mutex);
}

/// READ ///
#if defined(_WIN32) || defined(_WIN64)
     DWORD WINAPI serverRead(LPVOID arg) {
        TCP_CLIENT* client = (TCP_CLIENT*)arg;

        while (running && client->socket != INVALID_SOCKET) {
            I8 packetId = 0;
            if (recv(client->socket, &packetId, 1, 0) <= 0) {
                println("Data read failed");
                removeClient(client->id);
                return 0;
            }

            U16 size = getServerPacketSize(packetId);
            if (size <= 0) {
                printf("Invalid packet %i size: %i\n", packetId, size);
                removeClient(client->id);
                return 0;
            }

            U8* dataBuffer = malloc(size);
            U32 totalBytesRead = 0;
            while (totalBytesRead < size) {
                I32 bytesRead = recv(client->socket, (I8*)(dataBuffer + totalBytesRead), size - totalBytesRead, 0);
                totalBytesRead += bytesRead;

                if (bytesRead <= 0) {
                    println("Data read failed");
                    removeClient(client->id);
                    return 0;
                }
            }

            void (*f)(TCP_CLIENT*, U8*) = getServerPacketFunction(packetId);
            if (f != NULL) 
                f(client, dataBuffer);
            else
                free(dataBuffer);
        }

        return 0;
     }
#else
    void* serverRead(void* arg) {
        TCP_CLIENT* client = (TCP_CLIENT*)arg;

        while (running && client->socket >= 0) {
            I8 packetId = 0;
            if (recv(client->socket, &packetId, 1, 0) <= 0) {
                println("Data read failed");
                removeClient(client->id);
                return NULL;
            }
            U16 size = getServerPacketSize(packetId);
            if (size <= 0) {
                printf("Invalid packet %i size: %i\n", packetId, size);
                removeClient(client->id);
                return NULL;
            }

            U8* dataBuffer = malloc(size);
            U32 totalBytesRead = 0;
            while (totalBytesRead < size) {
                I32 bytesRead = recv(client->socket, dataBuffer + totalBytesRead, size - totalBytesRead, 0);
                totalBytesRead += bytesRead;

                if (bytesRead <= 0) {
                    println("Data read failed");
                    removeClient(client->id);
                    return NULL;
                }
            }

            void (*f)(TCP_CLIENT*, U8*) = getServerPacketFunction(packetId);
            if (f != NULL) 
                f(client, dataBuffer);
            else
                free(dataBuffer);
        }

        return NULL;
    }
#endif

/// WRITE ///
#if defined(_WIN32) || defined(_WIN64)
    void serverBroadcastWIN(U8* buffer, U32 size) {
        for (U32 i = 0; i < MAX_TCP_CLIENT; i++) {
            if (tcpClients[i] == NULL) continue;
            send(tcpClients[i]->socket, (I8*)buffer, size, 0);
        }
    }
    void serverWriteWIN(TCP_CLIENT* client, U8* buffer, U32 size) {
        if (client == NULL) return;
        send(client->socket, (I8*)buffer, size, 0);
    }
#else
    void serverBroadcastPOSIX(U8* buffer, U32 size) {
        for (U32 i = 0; i < MAX_TCP_CLIENT; i++) {
            if (tcpClients[i] == NULL) continue;
            send(tcpClients[i]->socket, buffer, size, 0);
        }
    }
    void serverWritePOSIX(TCP_CLIENT* client, U8* buffer, U32 size) {
        if (client == NULL) return;
        send(client->socket, buffer, size, 0);
    }
#endif

void serverBroadcast(U8* buffer, U32 size) {
    mutexLock(&mutex);
        #if defined(_WIN32) || defined(_WIN64)
            serverBroadcastWIN(buffer, size);
        #else
            serverBroadcastPOSIX(buffer, size);
        #endif
    mutexUnlock(&mutex);
}


void _serverWrite(TCP_CLIENT* client, U8* buffer, U32 size) {
    #if defined(_WIN32) || defined(_WIN64)
        serverWriteWIN(client, buffer, size);
    #else
        serverWritePOSIX(client, buffer, size);
    #endif
}

void serverWrite(TCP_CLIENT* client, U8* buffer, U32 size) {
    mutexLock(&mutex);
        #if defined(_WIN32) || defined(_WIN64)
            serverWriteWIN(client, buffer, size);
        #else
            serverWritePOSIX(client, buffer, size);
        #endif
    mutexUnlock(&mutex);
}

/// ACCEPT ///
#if defined(_WIN32) || defined(_WIN64)
    void serverAcceptWIN(void) {
        struct sockaddr_in clientAddress;
        socklen_t clientLength;
        SOCKET clientSocket = 0;

        clientLength = sizeof(clientAddress);
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientLength);
        if (clientSocket == INVALID_SOCKET) {
            println("Client accept failed");
            return;
        }

        TCP_CLIENT* client = addClient(clientSocket);
        HANDLE thread = startThread(serverRead, client);
        client->thread = thread;
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
            return;
        }

        TCP_CLIENT* client = addClient(clientSocket);
        pthread_t thread = startThread(serverRead, client);
        client->thread = thread;
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
        if (listen(serverSocket, TCP_LISTENT_QUEUE_SIZE) == SOCKET_ERROR) {
            println("Server listen failed");
            serverClean();
            return;
        }
        
        while (running) {
            serverAccept();
        }
    }
#else
    void serverListenPOSIX(void) {
        if (listen(serverSocket, TCP_LISTENT_QUEUE_SIZE) < 0) {
            println("Server listen failed");
            serverClean();
            return;
        }
        
        while (running) {
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
        for (U32 i = 0; i < MAX_TCP_CLIENT; i++) {
            if (tcpClients[i] == NULL) continue;
            closesocket(tcpClients[i]->socket);
            joinThread(tcpClients[i]->thread);
            free(tcpClients[i]);
            tcpClients[i] = NULL;
        }

        running = 0;
        closesocket(serverSocket);
        WSACleanup();
        serverSocket = 0;
    }
#else
    void serverCleanPOSIX(void) {
        for (U32 i = 0; i < MAX_TCP_CLIENT; i++) {
            if (tcpClients[i] == NULL) continue;
            close(tcpClients[i]->socket);
            joinThread(tcpClients[i]->thread);
            free(tcpClients[i]);
            tcpClients[i] = NULL;
        }

        running = 0;
        close(serverSocket);
        serverSocket = 0;
    }
#endif

void serverClean(void) {
    if (status == 0) return;
    status = 0;
    mutexDestroy(&mutex);

    #if defined(_WIN32) || defined(_WIN64)
        serverCleanWIN();
    #else
        serverCleanPOSIX();
    #endif

    free(tcpClients);
}

/// INIT ///
#if defined(_WIN32) || defined(_WIN64)
    void serverInitWIN(void) {
        struct sockaddr_in serverAddress;
        WSADATA wsaData;

        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            println("WSAStartup failed");
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
            return;
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
            return;
        }

        serverListen();
    }
#endif

void serverInit(void) {
    if (status == 1) return;
    status = 1;
    mutexCreate(&mutex);

    tcpClients = malloc(sizeof(TCP_CLIENT*) * MAX_TCP_CLIENT);
    for (U32 i = 0; i < MAX_TCP_CLIENT; i++) tcpClients[i] = NULL;

    #if defined(_WIN32) || defined(_WIN64)
        serverInitWIN();
    #else
        serverInitPOSIX();
    #endif
}
