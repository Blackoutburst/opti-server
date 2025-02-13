#pragma once

#if defined(_WIN32) || defined(_WIN64)
    #include <synchapi.h>
#else
    #include <pthread.h>
#endif

void mutexCreate(void* mutex);
void mutexDestroy(void* mutex);
void mutexLock(void* mutex);
void mutexUnlock(void* mutex);
