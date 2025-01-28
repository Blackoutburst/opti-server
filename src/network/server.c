#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils/ioUtils.h"
#include "network/server.h"

static U8 running = 0;

#if defined(_WIN32) || defined(_WIN64)
    static I32 serverSocket = NULL;
#else
    static I32 serverSocket = 0;
#endif

/// LISTEN ///
#if defined(_WIN32) || defined(_WIN64)
    void serverListenWIN(void) {
        while (running) {
        }
    }
#else
    void serverListenPOSIX(void) {
        while (running) {
            if (listen(serverSocket, 1) < 0) {
                println("Server listen failed");
                serverClean();
            }
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
    }
#else
    void serverCleanPOSIX(void) {
        running = 0;
        close(serverSocket);
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
