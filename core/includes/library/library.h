#pragma once

#include <windows.h>
#include "utils/types.h"

typedef struct library LIBRARY;
struct library {
    HINSTANCE instanceLib;
    U8* name;
    U8 isValid;
};

LIBRARY libraryLoad(const I8* name);
void libraryFree(const LIBRARY* dll);
void* libraryGet(const LIBRARY* lib, const I8* functionName);
