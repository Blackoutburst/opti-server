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

    if (worldGetChunk(client, chunk->position.x, chunk->position.y, chunk->position.z)) return;

    VECTORI elem = { chunk->position.x, chunk->position.y, chunk->position.z };
    insert(&client->chunks, chunkHash(chunk->position.x, chunk->position.y, chunk->position.z), elem);
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

    I32 minX = px - rd;
    I32 maxX = px + rd;
    I32 minY = py - rd;
    I32 maxY = py + rd;
    I32 minZ = pz - rd;
    I32 maxZ = pz + rd;

    CHUNK** chunksToAdd = malloc(sizeof(CHUNK*) * CUBE(2 * client->renderDistance));
    U32 addIndex = 0;

    dbGetChunksInRegion(client, minX, maxX, minY, maxY, minZ, maxZ);

    for (I32 x = px - rd; x < px + rd; x += CHUNK_SIZE) {
    for (I32 y = py - rd; y < py + rd; y += CHUNK_SIZE) {
    for (I32 z = pz - rd; z < pz + rd; z += CHUNK_SIZE) {
        U8** data = get(&client->dbChunks, chunkHash(x, y, z));
        if (y < -32 || worldGetChunk(client, x, y, z)) {
            if (data != NULL) free(*data);
            continue;
        }

        CHUNK* c = NULL;
        if (data == NULL) {
            c = chunkCreate(x, y, z);
            chunksToAdd[addIndex] = c;
            addIndex++;

            worldAddChunk(client, c);

            if (chunkIsEmpty(c)) {
                continue;
            }
            if (chunkIsMonotype(c)) {
                clientSendMonotypeChunk(client, c);
                continue;
            }
            clientSendChunk(client, c);
        } else {
            c = chunkAssemble(x, y, z, *data);
            worldAddChunk(client, c);

            if (chunkIsEmpty(c)) {
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
        }
    }}}

    clear(&client->dbChunks);

    if (addIndex) dbAddChunks(chunksToAdd, addIndex);
    free(chunksToAdd);
}

