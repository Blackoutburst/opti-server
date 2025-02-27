#pragma once

#include "utils/types.h"

typedef struct library LIBRARY;

#if defined(_WIN32) || defined(_WIN64)

#include <windows.h>

struct library {
    HINSTANCE handle;
    I8* name;
    U8 isValid;
};

#else

struct library {
    void* handle;
    I8* name;
    U8 isValid;
};

#endif

LIBRARY libraryLoad(const I8* name);
void libraryFree(LIBRARY* dll);
void* libraryGet(const LIBRARY* lib, const I8* functionName);
