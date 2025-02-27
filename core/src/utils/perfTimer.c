#include "utils/perfTimer.h"
#include "utils/logger.h"

#if defined(_WIN32) || defined(_WIN64)

#include <windows.h>

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

#else

#include <time.h>
#include <string.h>

struct timespec start_time;
I8 name[128] = {0};

void perfTimerBegin(const I8* _name) {
    strcpy(name, _name);

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_time);
}

// call this function to end a timer, returning nanoseconds elapsed as a long
void perfTimerEnd(void) {
    struct timespec end_time;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_time);

    I64 diffInNanos = (end_time.tv_sec - start_time.tv_sec) * (I64)1e9 + (end_time.tv_nsec - start_time.tv_nsec);

    logD("[TIMER] %s - %.4f ms", name, (double)diffInNanos / 1e6);
}

#endif
