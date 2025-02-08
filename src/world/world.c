#include <stdlib.h>
#include <stdio.h>
#include "utils/math.h"
#include "world/world.h"
#include "network/client.h"
#include "utils/math.h"
#include "database/database.h"

U8 worldGetChunk(TCP_CLIENT* client, I32 x, I32 y, I32 z) {
    if (client == NULL) return 0;

    return (get(&client->chunks, chunkHash(x, y, z)) != NULL);
}

void worldAddChunk(TCP_CLIENT* client, CHUNK* chunk) {
    if (client == NULL) return;

    VECTORI elem = { chunk->position.x, chunk->position.y, chunk->position.z };
    insert(&client->chunks, chunkHash(chunk->position.x, chunk->position.y, chunk->position.z), elem);
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

    erase(&client->chunks, chunkHash(x, y, z));
}

void worldRemoveChunkOutOfRenderDistance(TCP_CLIENT* client) {
    if (client == NULL) return;

    I32 px = TO_CHUNK_POS((I32)client->position.x);
    I32 py = TO_CHUNK_POS((I32)client->position.y);
    I32 pz = TO_CHUNK_POS((I32)client->position.z);
    I32 rd = client->renderDistance * CHUNK_SIZE;

    size_t count = size(&client->chunks);
    U32* keysToRemove = malloc(sizeof(U32) * count);
    size_t removeCount = 0;

    for_each(&client->chunks, key, value) {
         if ( value->x < px - rd || value->x >= px + rd ||
              value->y < py - rd || value->y >= py + rd ||
              value->z < pz - rd || value->z >= pz + rd )
         {
             keysToRemove[removeCount++] = *key;
         }
    }

    for (size_t i = 0; i < removeCount; i++) {
         erase(&client->chunks, keysToRemove[i]);
    }
    free(keysToRemove);
}

void worldUpdateClientChunk(TCP_CLIENT* client) {
    if (client == NULL) return;

    worldRemoveChunkOutOfRenderDistance(client);

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

