#pragma once

#include "utils/types.h"

#if defined(_WIN32) || defined(_WIN64)
    #include <winsock2.h>
#else
    #include <pthread.h>
#endif

#if defined(_WIN32) || defined(_WIN64)
    HANDLE startThread(DWORD WINAPI (*method)(LPVOID), void *arg);
    void joinThread(HANDLE thread);
#else
    pthread_t startThread(void* (*method)(void *), void *arg);
    void joinThread(pthread_t thread);
#endif
