#include <stdio.h>
#include <stdlib.h>

#include "utils/types.h"
#include "network/server.h"
#include "database/database.h"

static U8 renderDistance = 2;

U8 getServerMaxRenderDistance(void) {
    return renderDistance;
}

void updateRenderDistance(I8* arg) {
    renderDistance = atoi(arg);
}

I32 main(I32 argc, I8** argv) {
    if (argc > 1) updateRenderDistance(argv[1]);

    printf("Starting server with a max render distance of: %i\n", renderDistance);

    dbInit();
    serverInit();
    serverClean();

    printf("Shutting down server\n");
    return 0;
}
