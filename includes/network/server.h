#pragma once

#include "utils/types.h"

#define TCP_PORT 15000
#define BUFFER_SIZE 5000

#if defined(_WIN32) || defined(_WIN64)
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <synchapi.h>
#else
    #include <unistd.h>
    #include <pthread.h>
#endif


#if defined(_WIN32) || defined(_WIN64)
    void serverCleanWIN(void);
    void serverInitWIN(void);
#else
    void serverCleanPOSIX(void);
    void serverInitPOSIX(void);
#endif

void serverClean(void);
void serverInit(void);
