#include <stdio.h>
#include "library/library.h"
#include "utils/string.h"
#include "utils/logger.h"

const LIBRARY invalidLibrary = {NULL, 0, 0};

#if defined(_WIN32) || defined(_WIN64)

LIBRARY libraryLoad(const I8* name) {
    LIBRARY lib = invalidLibrary;

    lib.handle = LoadLibrary(name);
    if (lib.handle == NULL) {
        logE("Couldn't load library");
        return invalidLibrary;
    }

    lib.name = copyString(name);
    lib.isValid = 1;

    return lib;
}

void* libraryGet(const LIBRARY* lib, const I8* functionName) {
    void* func = GetProcAddress(lib->handle, functionName);

    if (func == NULL) {
        logE("Loading function %s", functionName);
    }

    return func;
}

void libraryFree(LIBRARY* lib) {
    int r = FreeLibrary(lib->handle);

    lib->isValid = false;

    if (r == 0) {
        logE("Failed freeing library");
    }
}

#else

#include <dlfcn.h>

LIBRARY libraryLoad(const I8* name) {
    LIBRARY lib = invalidLibrary;

    lib.handle = dlopen(name, RTLD_LAZY);
    if (lib.handle == NULL) {
        logE("Couldn't load library");
        return invalidLibrary;
    }

    lib.name = copyString(name);
    lib.isValid = 1;

    return lib;

}

void* libraryGet(const LIBRARY* lib, const I8* functionName) {
    void* func = dlsym(lib->handle, functionName);

    if (func == NULL) {
        logE("Loading function %s", functionName);
    }

    return func;
}

void libraryFree(LIBRARY* lib) {
    if (lib->handle != NULL) {
        dlclose(lib->handle);
    }

    lib->isValid = false;
}

#endif
