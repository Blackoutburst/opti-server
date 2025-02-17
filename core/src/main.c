#include <stdio.h>
#include <stdlib.h>

#include "utils/types.h"
#include "utils/logger.h"
#include "network/server.h"
#include "database/database.h"
#include "utils/args.h"

#if defined(_WIN32) || defined(_WIN64)
    #include "library/library.h"
#endif

#if defined(_WIN32) || defined(_WIN64)
    void loadLibraries(void) {
        LIBRARY lib_worldgen = libraryLoad("worldgen.dll");

        chunkSetGenChunkFunction((worldgen_genChunk)libraryGet(&lib_worldgen, "genChunk"));
        // libraryFree(&lib_worldgen);
    }
#endif

I32 main(I32 argc, I8** argv) {
    if (argc > 1) argsParse(argc, argv);
    logI("Starting server with a max render distance of: %i, database type: %s", argsGetRenderDistance(), argsGetDbType() == DB_FILE ? "file" : "ram");

    #if defined(_WIN32) || defined(_WIN64)
        loadLibraries();
    #endif

    dbInit();
    serverInit();
    serverClean();

    logI("Shutting down server");
    return 0;
}
