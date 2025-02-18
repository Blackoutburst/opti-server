#if defined(_WIN32) || defined(_WIN64)

#include <windows.h>
#include "utils/perfTimer.h"
#include "utils/logger.h"

// #include <threads.h>
// thread_local

static LARGE_INTEGER frequency;
static LARGE_INTEGER start;
static LARGE_INTEGER end;
static F64 interval;
I8 name[128] = {0};

void perfTimerBegin(const I8* _name) {
    strcpy(name, _name);

    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);
}

void perfTimerEnd(void) {
    QueryPerformanceCounter(&end);
    interval = (F64) (end.QuadPart - start.QuadPart) / frequency.QuadPart; // seconds
    interval *= 1000.0; // convert to milliseconds

    logD("[TIMER] %s - %.4f ms", name, interval);
}

#endif
