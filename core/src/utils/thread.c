#include "utils/thread.h"
#include "utils/logger.h"

#if defined(_WIN32) || defined(_WIN64)
    HANDLE startThread(DWORD WINAPI (*method)(LPVOID), void *arg) {
        HANDLE thread = CreateThread(NULL, 0, method, arg, 0, NULL);

        if (thread == NULL) {
            logW("Thread creation failed");
            return NULL;
        }

        logT("Thread created successfully");
        return thread;
    }

    void joinThread(HANDLE thread) {
        WaitForSingleObject(thread, INFINITE);
    }
#else
    pthread_t startThread(void* (*method)(void *), void *arg) {
        pthread_t thread;

        if (pthread_create(&thread, NULL, method, arg)) {
            logW("Thread creation failed");
            return 0;
        }

        logT("Thread created successfully");
        return thread;
    }

    void joinThread(pthread_t thread) {
        pthread_join(thread, 0);
    }
#endif
