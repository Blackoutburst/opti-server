#pragma once

#include "utils/types.h"
#include "utils/vector.h"
#include "world/chunk.h"
#include "cc/cc.h"

#if defined(_WIN32) || defined(_WIN64)
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <pthread.h>
#endif

#define TCP_PORT 20000
#define TCP_LISTENT_QUEUE_SIZE 8
#define MAX_TCP_CLIENT 16
#define BUFFER_SIZE 5000

typedef struct tcpClient TCP_CLIENT;

struct tcpClient {
    U32 id;
    #if defined(_WIN32) || defined(_WIN64)
        SOCKET socket;
        HANDLE thread;
    #else
        I32 socket;
        pthread_t thread;
    #endif
    VECTORF position;
    VECTORF chunkPosition;
    F32 yaw;
    F32 pitch;
    map(U32, VECTORI) chunks;
    map(U32, U8*) dbChunks;
    U8 renderDistance;
    U8 name[64];
};

#if defined(_WIN32) || defined(_WIN64)
    TCP_CLIENT* addClient(SOCKET socket);
    TCP_CLIENT* getClientBysocket(SOCKET socket);
    DWORD WINAPI serverRead(LPVOID arg);
    void serverBroadcastWIN(U8* buffer, U32 size);
    void serverWriteWIN(TCP_CLIENT* client, U8* buffer, U32 size);
    void serverAcceptWIN(void);
    void serverListenWIN(void);
    void serverCleanWIN(void);
    void serverInitWIN(void);
#else
    TCP_CLIENT* addClient(I32 socket);
    TCP_CLIENT* getClientBysocket(I32 socket);
    void* serverRead(void* arg);
    void serverBroadcastPOSIX(U8* buffer, U32 size);
    void serverWritePOSIX(TCP_CLIENT* client, U8* buffer, U32 size);
    void serverAcceptPOSIX(void);
    void serverListenPOSIX(void);
    void serverCleanPOSIX(void);
    void serverInitPOSIX(void);
#endif

U32 getClientId(void);
U8 recvAll(TCP_CLIENT* client, U8* buffer, U32 size, U32 bufferOffset);
void _serverSendAddEntity(TCP_CLIENT* client, TCP_CLIENT* entity);
void _serverSendRemoveEntity(TCP_CLIENT* client, U32 entityId);
TCP_CLIENT** getAllClients(void);
void removeClient(U32 id);
void serverBroadcast(U8* buffer, U32 size);
void _serverWrite(TCP_CLIENT* client, U8* buffer, U32 size);
void serverWrite(TCP_CLIENT* client, U8* buffer, U32 size);
void serverAccept(void);
void serverListen(void);
void serverClean(void);
void serverInit(void);
