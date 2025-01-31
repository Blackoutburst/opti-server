#include <stdlib.h>
#include "utils/math.h"
#include "world/world.h"

CHUNK* worldGetChunk(TCP_CLIENT* client, I32 x, I32 y, I32 z) {
    if (client == NULL) return NULL;

    U32 index = chunkHash(x, y, z) % CUBE(client->renderDistance);
    U32 start = index;

    while (1) {
        if (!client->chunks[index].used) {
            return NULL;
        }

        if (client->chunks[index].position.x == x && client->chunks[index].position.y == y && client->chunks[index].position.z == z) {
            return client->chunks[index].chunk;
        }

        index = (index + 1) % CUBE(client->renderDistance);

        if (index == start) {
            return NULL;
        }
    }
}

CHUNK* worldLoadChunk(TCP_CLIENT* client, I32 x, I32 y, I32 z) {
    if (client == NULL) return NULL;
    if (worldGetChunk(client, x, y, z) != NULL) return NULL;

    // TODO:
    // DB lookup + send if exist
    // if not in DB generate + send

    return NULL;
}

void worldUnloadChunk(TCP_CLIENT* client, I32 x, I32 y, I32 z) {
    if (client == NULL) return;

    U32 index = chunkHash(x, y, z) % CUBE(client->renderDistance);
    U32 start = index;

    while (1) {
        if (client->chunks[index].used &&
            client->chunks[index].position.x == x &&
            client->chunks[index].position.y == y &&
            client->chunks[index].position.z == z)
        {
            // TODO: DB save
            client->chunks[index].used = 0;
            chunkClean(client->chunks[index].chunk);
            client->chunks[index].chunk = NULL;

            return;
        }

        index = (index + 1) % CUBE(client->renderDistance);

        if (index == start) {
            return;
        }
    }
}

void worldRemoveChunkOutOfRenderDistance(TCP_CLIENT* client) {
    if (client == NULL) return;

    I32 px = TO_CHUNK_POS(client->position.x);
    I32 py = TO_CHUNK_POS(client->position.y);
    I32 pz = TO_CHUNK_POS(client->position.z);

    for (U32 i = 0; i < CUBE(client->renderDistance); i++) {
        if (!client->chunks[i].used) continue;

        if (abs(client->chunks[i].position.x - px) > (client->renderDistance * CHUNK_SIZE) ||
            abs(client->chunks[i].position.y - py) > (client->renderDistance * CHUNK_SIZE) ||
            abs(client->chunks[i].position.z - pz) > (client->renderDistance * CHUNK_SIZE))
        {
            worldUnloadChunk(client, client->chunks[i].position.x, client->chunks[i].position.y, client->chunks[i].position.z);
        }
    }
}

void worldUpdateClientChunk(TCP_CLIENT* client) {
    if (client == NULL) return;

    worldRemoveChunkOutOfRenderDistance(client);

    // TODO do for loop shit to load cube arround player
}

