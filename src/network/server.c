#include <stdio.h>
#include <stdlib.h>
#include "network/server.h"

/// CLEAN ///

#if defined(_WIN32) || defined(_WIN64)
    void serverCleanWIN(void) {

    }
#else
    void serverCleanPOSIX(void) {

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

    }
#endif

void serverInit(void) {
    #if defined(_WIN32) || defined(_WIN64)
        serverInitWIN();
    #else
        serverInitPOSIX();
    #endif
    printf("Server running on port [TCP:%i]\n", TCP_PORT);
}
