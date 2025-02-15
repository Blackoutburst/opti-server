#include <stdio.h>
#include <stdlib.h>

#include "utils/types.h"
#include "network/server.h"
#include "database/database.h"

#if defined(_WIN32) || defined(_WIN64)
    #include "library/library.h"
#endif

static U8 renderDistance = 2;

U8 getServerMaxRenderDistance(void) {
    return renderDistance;
}

void updateRenderDistance(I8* arg) {
    renderDistance = atoi(arg);
}

#if defined(_WIN32) || defined(_WIN64)
    void loadLibraries(void) {
        LIBRARY lib_worldgen = libraryLoad("worldgen.dll");

        chunkSetGenChunkFunction((worldgen_genChunk)libraryGet(&lib_worldgen, "genChunk"));
        // libraryFree(&lib_worldgen);
    }
#endif

I32 main(I32 argc, I8** argv) {
    if (argc > 1) updateRenderDistance(argv[1]);
    printf("Starting server with a max render distance of: %i\n", renderDistance);

    #if defined(_WIN32) || defined(_WIN64)
        loadLibraries();
    #endif

    dbInit();
    serverInit();
    serverClean();

    printf("Shutting down server\n");
    return 0;
}
