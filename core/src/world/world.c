#include <stdlib.h>
#include <stdio.h>
#include "utils/math.h"
#include "world/world.h"
#include "network/client.h"
#include "utils/math.h"
#include "database/database.h"
#include "utils/logger.h"
#include "utils/perfTimer.h"


U8 worldGetChunk(TCP_CLIENT* client, I32 x, I32 y, I32 z) {
    if (client == NULL) return 0;

    return (get(&client->chunks, ((VECTORI){x, y, z})) != NULL);
}

void worldAddChunk(TCP_CLIENT* client, CHUNK* chunk) {
    if (client == NULL) return;

    VECTORI elem = { chunk->position.x, chunk->position.y, chunk->position.z };
    insert(&client->chunks, chunk->position, elem);
}

void worldUnloadChunk(TCP_CLIENT* client, I32 x, I32 y, I32 z) {
    if (client == NULL) return;

    erase(&client->chunks, ((VECTORI){x, y, z}));
}

void worldRemoveChunkOutOfRenderDistance(TCP_CLIENT* client) {
    if (client == NULL) return;

    I32 px = TO_CHUNK_POS((I32)client->position.x);
    I32 py = TO_CHUNK_POS((I32)client->position.y);
    I32 pz = TO_CHUNK_POS((I32)client->position.z);
    I32 rd = client->renderDistance * CHUNK_SIZE;

    size_t count = size(&client->chunks);
    VECTORI* keysToRemove = malloc(sizeof(VECTORI) * count);
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
    I32 rd = ((I32)client->renderDistance) * CHUNK_SIZE;

    I32 oldbbminX = client->chunkPosition.x - rd;
    I32 oldbbmaxX = client->chunkPosition.x + rd;
    I32 oldbbminY = client->chunkPosition.y - rd;
    I32 oldbbmaxY = client->chunkPosition.y + rd;
    I32 oldbbminZ = client->chunkPosition.z - rd;
    I32 oldbbmaxZ = client->chunkPosition.z + rd;

    I32 newbbminX = px - rd;
    I32 newbbmaxX = px + rd;
    I32 newbbminY = py - rd;
    I32 newbbmaxY = py + rd;
    I32 newbbminZ = pz - rd;
    I32 newbbmaxZ = pz + rd;

    I32 a_dx = (px - client->chunkPosition.x);
    I32 a_dy = (py - client->chunkPosition.y);
    I32 a_dz = (pz - client->chunkPosition.z);

    I32 bx = a_dx;// / 16;
    I32 by = a_dy;// / 16;
    I32 bz = a_dz;// / 16;

    // logD("chunkPosition %d %d %d", client->chunkPosition.x, client->chunkPosition.y, client->chunkPosition.z);
    // logD("%d %d %d", bx, by, bz);
    // logD("p: %d %d %d", px, py, pz);
    // logD("chunkPosition: %d %d %d", client->chunkPosition.x, client->chunkPosition.y, client->chunkPosition.z);
    // logD("a_d: %d %d %d", a_dx, a_dy, a_dz);

    // logD("newbbminmaxX %d %d", newbbminX, newbbmaxX);
    // logD("newbbminmaxY %d %d", newbbminY, newbbmaxY);
    // logD("newbbminmaxZ %d %d", newbbminZ, newbbmaxZ);

    // perfTimerBegin("dbGetChunksInRegions");

    if (bx != 0) {
        I32 minX = bx > 0 ? MAX(newbbminX, oldbbmaxX) : MIN(newbbminX, oldbbminX);
        I32 maxX = bx > 0 ? MAX(newbbmaxX, oldbbmaxX) : MIN(newbbmaxX, oldbbminX);

        // logD("minX: %d, maxX: %d - %d", minX, maxX, maxX - minX);
        // int volume = (maxX - minX) * (newbbmaxY - newbbminY) * (newbbmaxZ - newbbminZ);
        // logD("Volume X: %d", volume / 4096);

        dbGetChunksInRegion(client, minX, maxX, newbbminY, newbbmaxY, newbbminZ, newbbmaxZ);
    }

    if (by != 0) {
        I32 minY = by > 0 ? MAX(newbbminY, oldbbmaxY) : MIN(newbbminY, oldbbminY);
        I32 maxY = by > 0 ? MAX(newbbmaxY, oldbbmaxY) : MIN(newbbmaxY, oldbbminY);

        // logD("minY: %d, maxY: %d - %d", minY, maxY, maxY - minY);
        // int volume = (newbbmaxX - newbbminX) * (maxY - minY) * (newbbmaxZ - newbbminZ);
        // logD("Volume Y: %d", volume / 4096);

        dbGetChunksInRegion(client, newbbminX, newbbmaxX, minY, maxY, newbbminZ, newbbmaxZ);
    }

    // BUG ??: Z axis seems to be 2~3x slower than X axis and Y axis
    if (bz != 0) {
        I32 minZ = bz > 0 ? MAX(newbbminZ, oldbbmaxZ) : MIN(newbbminZ, oldbbminZ);
        I32 maxZ = bz > 0 ? MAX(newbbmaxZ, oldbbmaxZ) : MIN(newbbmaxZ, oldbbminZ);

        // logD("minZ: %d, maxZ: %d - %d", minZ, maxZ, maxZ - minZ);
        // int volume = (newbbmaxX - newbbminX) * (newbbmaxY - newbbminY) * (maxZ - minZ);
        // logD("Volume Z: %d", volume / 4096);

        dbGetChunksInRegion(client, newbbminX, newbbmaxX, newbbminY, newbbmaxY, minZ, maxZ);
    }

    // perfTimerEnd();

    CHUNK** chunksToAdd = malloc(sizeof(CHUNK*) * CUBE(2 * client->renderDistance));
    U32 addIndex = 0;

    // Do not use <= or it will not match the above code
    for (I32 x = px - rd; x < px + rd; x += CHUNK_SIZE) {
    for (I32 y = py - rd; y < py + rd; y += CHUNK_SIZE) {
    for (I32 z = pz - rd; z < pz + rd; z += CHUNK_SIZE) {
        U8** data = get(&client->dbChunks, ((VECTORI){x, y, z}));
        if (y < -384 || worldGetChunk(client, x, y, z)) {

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

    // if (generatedChunks) logW("generatedChunks");

    clear(&client->dbChunks);

    if (addIndex) dbAddChunks(chunksToAdd, addIndex);
    free(chunksToAdd);
}
