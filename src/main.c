#include <stdio.h>
#include <stdlib.h>

#include "utils/types.h"

void updateRenderDistance(U8* renderDistance, I8* arg) {
    *renderDistance = atoi(arg);
}

I32 main(I32 argc, I8** argv) {
    U8 renderDistance = 2;
    if (argc > 1) updateRenderDistance(&renderDistance, argv[1]);

    printf("Starting server with a max render distance of: %i\n", renderDistance);
    return 0;
}

