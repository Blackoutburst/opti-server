#include "utils/thread.h"
#include "utils/ioUtils.h"

#if defined(_WIN32) || defined(_WIN64)
    HANDLE createThread(DWORD WINAPI (*method)(LPVOID), void *arg) {
        HANDLE thread = CreateThread(NULL, 0, method, arg, 0, NULL);

        if (thread == NULL) {
            printf("World gen thread %i creation failed\n", i);
            return;
        }

        return thread;
    }

    void joinThread(HANDLE thread) {
        WaitForSingleObject(thread, INFINITE);
    }
#else
    pthread_t createThread(void* (*method)(void *), void *arg) {
        pthread_t thread;

        if (pthread_create(&thread, NULL, method, arg)) {
            println("Thread creation failed");
            return NULL;
        }

        return thread;
    }

    void joinThread(pthread_t thread) {
        pthread_join(thread, 0);
    }
#endif
