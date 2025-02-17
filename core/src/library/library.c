
#if defined(_WIN32) || defined(_WIN64)
    #include <stdio.h>
    #include "utils/string.h"
    #include "utils/logger.h"
    #include "library/library.h"

    const LIBRARY invalidLibrary = {NULL, 0, 0};

    LIBRARY libraryLoad(const I8* name) {
        LIBRARY lib = invalidLibrary;

        lib.instanceLib = LoadLibrary(name);
        if (lib.instanceLib == NULL) {
            logE("Couldn't load library");
            return invalidLibrary;
        }

        lib.name = copyString(name);
        lib.isValid = 1;

        return lib;
    }

    void* libraryGet(const LIBRARY* lib, const I8* functionName) {
        void* func = GetProcAddress(lib->instanceLib, functionName);

        if (func == NULL) {
            logE("Loading function %s", functionName);
        }

        return func;
    }

    void libraryFree(const LIBRARY* lib) {
        int r = FreeLibrary(lib->instanceLib);

        if (r == 0) {
            logE("Failed freeing library");
        }
    }
#endif
