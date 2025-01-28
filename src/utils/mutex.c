#include "utils/mutex.h"

void mutexCreate(void* mutex) {
    #if defined(_WIN32) || defined(_WIN64)
        InitializeCriticalSection(mutex);
    #else
        pthread_mutex_init(mutex, NULL);
    #endif
}

void mutexDestroy(void* mutex) {
    #if defined(_WIN32) || defined(_WIN64)
        DeleteCriticalSection(mutex);
    #else
        pthread_mutex_destroy(mutex);
    #endif
}

void mutexLock(void* mutex) {
    #if defined(_WIN32) || defined(_WIN64)
        EnterCriticalSection(mutex);
    #else
        pthread_mutex_lock(mutex);
    #endif
}

void mutexUnlock(void* mutex) {
    #if defined(_WIN32) || defined(_WIN64)
        LeaveCriticalSection(mutex);
    #else
        pthread_mutex_unlock(mutex);
    #endif
}
