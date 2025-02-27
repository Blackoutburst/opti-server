#include <stdio.h>
#include <stdlib.h>

#include "utils/types.h"
#include "utils/logger.h"
#include "network/server.h"
#include "database/database.h"
#include "utils/args.h"

#include "library/library.h"

void loadLibraries(void) {
#if defined(_WIN32) || defined(_WIN64)
    const char* worldgenLibPath = "./worldgen.dll";
#else
    const char* worldgenLibPath = "./worldgen.so";
#endif

    LIBRARY lib_worldgen = libraryLoad(worldgenLibPath);

    chunkSetGenChunkFunction((worldgen_genChunk)libraryGet(&lib_worldgen, "genChunk"));
    // libraryFree(&lib_worldgen);
}

I32 main(I32 argc, I8** argv) {
    if (argc > 1) argsParse(argc, argv);
    logI("Starting server with a max render distance of: %i, database type: %s", argsGetRenderDistance(), argsGetDbType() == DB_FILE ? "file" : "ram");

    loadLibraries();

    dbInit();
    serverInit();
    serverClean();

    logI("Shutting down server");
    return 0;
}
