#include <stdlib.h>
#include "utils/math.h"
#include "world/world.h"
#include "network/client.h"

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

void worldAddChunk(TCP_CLIENT* client, CHUNK* chunk) {
    if (client == NULL) return;

    U32 index = chunkHash(chunk->position.x, chunk->position.y, chunk->position.z) % CUBE(client->renderDistance);
    U32 start = index;

    while (1) {
        if (!client->chunks[index].used) {
            client->chunks[index].position = chunk->position;
            client->chunks[index].chunk = chunk;
            client->chunks[index].used = 1;

            return;
        } else if (client->chunks[index].used &&
                  client->chunks[index].position.x == chunk->position.x &&
                  client->chunks[index].position.y == chunk->position.y &&
                  client->chunks[index].position.z == chunk->position.z)
        {
            chunkClean(client->chunks[index].chunk);
            client->chunks[index].chunk = chunk;
            return;
        }

        index = (index + 1) % CUBE(client->renderDistance);

        if (index == start) {
            return;
        }
    }
}

CHUNK* worldLoadChunk(TCP_CLIENT* client, I32 x, I32 y, I32 z) {
    if (client == NULL) return NULL;
    if (worldGetChunk(client, x, y, z) != NULL) return NULL;

    // TODO:
    // DB lookup + send if exist
    // if not in DB generate + send
    CHUNK* c = chunkCreate(x, y, z);
    worldAddChunk(client, c);

    return c;
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
            client->chunks[index].position.x = 0;
            client->chunks[index].position.y = 0;
            client->chunks[index].position.z = 0;
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

    I32 px = TO_CHUNK_POS(client->position.x);
    I32 py = TO_CHUNK_POS(client->position.y);
    I32 pz = TO_CHUNK_POS(client->position.z);
    I32 rd = client->renderDistance * CHUNK_SIZE;

    for (I32 x = px - rd; x < px + rd; x += CHUNK_SIZE) {
    for (I32 y = py - rd; y < py + rd; y += CHUNK_SIZE) {
    for (I32 z = pz - rd; z < pz + rd; z += CHUNK_SIZE) {
        CHUNK* c = worldLoadChunk(client, x, y, z);
        if (c == NULL || chunkIsEmpty(c) || y < -32) continue;

        if (chunkIsMonotype(c)) {
            clientSendMonotypeChunk(client, c);
            continue;
        }
        clientSendChunk(client, c);
    }}}
}

