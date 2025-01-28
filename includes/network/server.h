#pragma once

#include "utils/types.h"

#define TCP_PORT 15000
#define TCP_LISTENT_QUEUE_SIZE 8
#define BUFFER_SIZE 5000

#if defined(_WIN32) || defined(_WIN64)
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <synchapi.h>
#else
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <pthread.h>
#endif


#if defined(_WIN32) || defined(_WIN64)
    DWORD WINAPI serverRead(LPVOID arg);
    void serverWriteWIN(U8* buffer);
    void serverAcceptWIN(void);
    void serverListenWIN(void);
    void serverCleanWIN(void);
    void serverInitWIN(void);
#else
    void* serverRead(void* arg);
    void serverWritePOSIX(U8* buffer);
    void serverAcceptPOSIX(void);
    void serverListenPOSIX(void);
    void serverCleanPOSIX(void);
    void serverInitPOSIX(void);
#endif

void serverWrite(U8* buffer);
void serverAccept(void);
void serverListen(void);
void serverClean(void);
void serverInit(void);
