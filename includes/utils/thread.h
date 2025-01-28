#pragma once

#include "utils/types.h"

#if defined(_WIN32) || defined(_WIN64)
    #include <synchapi.h>
#else
    #include <pthread.h>
#endif

#if defined(_WIN32) || defined(_WIN64)
    HANDLE createThread(DWORD WINAPI (*method)(LPVOID), void *arg);
    void joinThread(HANDLE thread);
#else
    pthread_t createThread(void* (*method)(void *), void *arg);
    void joinThread(pthread_t thread);
#endif
