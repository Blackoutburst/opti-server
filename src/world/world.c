#include <stdlib.h>
#include <stdio.h>
#include "utils/math.h"
#include "world/world.h"
#include "network/client.h"
#include "utils/math.h"
#include "database/database.h"

void worldRehashChunks(TCP_CLIENT* client) {
    if (client == NULL) return;

    CHUNK_HASHMAP* newChunks = malloc(sizeof(CHUNK_HASHMAP) * CUBE(client->renderDistance * 2));

    for (U32 i = 0; i < CUBE(client->renderDistance * 2); i++) {
        newChunks[i].used = 0;
        newChunks[i].position.x = 0;
        newChunks[i].position.y = 0;
        newChunks[i].position.z = 0;
    }

    for (U32 i = 0; i < CUBE(client->renderDistance * 2); i++) {
        if (!client->chunks[i].used) continue;

        I32 x = client->chunks[i].position.x;
        I32 y = client->chunks[i].position.y;
        I32 z = client->chunks[i].position.z;

        U32 index = chunkHash(x, y, z) % CUBE(client->renderDistance * 2);

        while (newChunks[index].used) {
            index = (index + 1) % CUBE(client->renderDistance * 2);
        }

        newChunks[index] = client->chunks[i];
    }

    free(client->chunks);
    client->chunks = newChunks;
}


U8 worldGetChunk(TCP_CLIENT* client, I32 x, I32 y, I32 z) {
    if (client == NULL) return 0;

    U32 index = chunkHash(x, y, z) % CUBE(client->renderDistance * 2);
    U32 start = index;

    while (1) {
        if (!client->chunks[index].used) {
            return 0;
        }

        if (client->chunks[index].position.x == x && client->chunks[index].position.y == y && client->chunks[index].position.z == z) {
            return 1;
        }

        index = (index + 1) % CUBE(client->renderDistance * 2);

        if (index == start) {
            return 0;
        }
    }
}

void worldAddChunk(TCP_CLIENT* client, CHUNK* chunk) {
    if (client == NULL) return;

    U32 index = chunkHash(chunk->position.x, chunk->position.y, chunk->position.z) % CUBE(client->renderDistance * 2);
    U32 start = index;

    while (1) {
        if (!client->chunks[index].used) {
            client->chunks[index].position = chunk->position;
            client->chunks[index].used = 1;

            return;
        }

        index = (index + 1) % CUBE(client->renderDistance * 2);

        if (index == start) {
            return;
        }
    }
}

CHUNK* worldLoadChunk(TCP_CLIENT* client, I32 x, I32 y, I32 z) {
    if (client == NULL) return NULL;
    if (worldGetChunk(client, x, y, z)) return NULL;

    CHUNK* c = NULL;
    U8* data = dbGetChunkBlocks(x, y, z);
    if (data == NULL) {
        c = chunkCreate(x, y, z);
        dbAddChunk(c);
    } else {
        c = chunkAssemble(x, y, z, data);
    }
    
    worldAddChunk(client, c);

    return c;
}

void worldUnloadChunk(TCP_CLIENT* client, I32 x, I32 y, I32 z) {
    if (client == NULL) return;

    U32 index = chunkHash(x, y, z) % CUBE(client->renderDistance * 2);
    U32 start = index;

    while (1) {
        if (client->chunks[index].used &&
            client->chunks[index].position.x == x &&
            client->chunks[index].position.y == y &&
            client->chunks[index].position.z == z)
        {
            client->chunks[index].position.x = 0;
            client->chunks[index].position.y = 0;
            client->chunks[index].position.z = 0;
            client->chunks[index].used = 0;
            return;
        }

        index = (index + 1) % CUBE(client->renderDistance * 2);

        if (index == start) {
            return;
        }
    }
}

void worldRemoveChunkOutOfRenderDistance(TCP_CLIENT* client) {
    if (client == NULL) return;

    I32 px = TO_CHUNK_POS((I32)client->position.x);
    I32 py = TO_CHUNK_POS((I32)client->position.y);
    I32 pz = TO_CHUNK_POS((I32)client->position.z);
    I32 rd = client->renderDistance * CHUNK_SIZE;

    for (U32 i = 0; i < CUBE(client->renderDistance * 2); i++) {
        if (!client->chunks[i].used) continue;

        if (client->chunks[i].position.x < px - rd ||
            client->chunks[i].position.x >= px + rd ||
            client->chunks[i].position.y < py - rd ||
            client->chunks[i].position.y >= py + rd ||
            client->chunks[i].position.z < pz - rd ||
            client->chunks[i].position.z >= pz + rd)
        {
            worldUnloadChunk(client, client->chunks[i].position.x, client->chunks[i].position.y, client->chunks[i].position.z);
        }
    }
}

void worldUpdateClientChunk(TCP_CLIENT* client) {
    if (client == NULL) return;

    worldRemoveChunkOutOfRenderDistance(client);
    worldRehashChunks(client);

    I32 px = TO_CHUNK_POS((I32)client->position.x);
    I32 py = TO_CHUNK_POS((I32)client->position.y);
    I32 pz = TO_CHUNK_POS((I32)client->position.z);
    
    I32 rd = client->renderDistance * CHUNK_SIZE;

    for (I32 x = px - rd; x < px + rd; x += CHUNK_SIZE) {
    for (I32 y = py - rd; y < py + rd; y += CHUNK_SIZE) {
    for (I32 z = pz - rd; z < pz + rd; z += CHUNK_SIZE) {
        CHUNK* c = worldLoadChunk(client, x, y, z);
        if (c == NULL || chunkIsEmpty(c) || y < -32) {
            chunkClean(c);
            continue;
        }

        if (chunkIsMonotype(c)) {
            clientSendMonotypeChunk(client, c);
            chunkClean(c);
            continue;
        }
        clientSendChunk(client, c);
        chunkClean(c);
    }}}
}

