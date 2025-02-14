#include "library/library.h"
#include <stdio.h>
#include "utils/string.h"

const LIBRARY invalidLibrary = {NULL, 0, 0};

LIBRARY libraryLoad(const I8* name) {
    LIBRARY lib = invalidLibrary;

    lib.instanceLib = LoadLibrary(name);
    if (lib.instanceLib == NULL) {
        printf("[Error]: Couldn't load library\n");
        return invalidLibrary;
    }

    lib.name = copyString(name);
    lib.isValid = 1;

    return lib;
}

void* libraryGet(const LIBRARY* lib, const I8* functionName) {
    void* func = GetProcAddress(lib->instanceLib, functionName);

    if (func == NULL) {
        printf("[Error]: Loading function %s\n", functionName);
    }

    return func;
}

void libraryFree(const LIBRARY* lib) {
    int r = FreeLibrary(lib->instanceLib);

    if (r == 0) {
        printf("[Error]: Failed freeing library\n");
    }
}
