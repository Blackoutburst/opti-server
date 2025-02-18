#include <stdlib.h>
#include <stdio.h>
#include "utils/math.h"
#include "world/world.h"
#include "network/client.h"
#include "utils/math.h"
#include "database/database.h"
#include "utils/perfTimer.h"
#include "utils/logger.h"

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

    // I32 oldbbminX = client->chunkPosition.x - rd;
    // I32 oldbbmaxX = client->chunkPosition.x + rd;
    // I32 oldbbminY = client->chunkPosition.y - rd;
    // I32 oldbbmaxY = client->chunkPosition.y + rd;
    // I32 oldbbminZ = client->chunkPosition.z - rd;
    // I32 oldbbmaxZ = client->chunkPosition.z + rd;

    // I32 newbbminX = px - rd;
    // I32 newbbmaxX = px + rd;
    // I32 newbbminY = py - rd;
    // I32 newbbmaxY = py + rd;
    // I32 newbbminZ = pz - rd;
    // I32 newbbmaxZ = pz + rd;

    // I32 px_abs = abs(px);
    // I32 py_abs = abs(py);
    // I32 pz_abs = abs(pz);
    // I32 chunkPositionX_abs = abs(client->chunkPosition.x);
    // I32 chunkPositionY_abs = abs(client->chunkPosition.y);
    // I32 chunkPositionZ_abs = abs(client->chunkPosition.z);
    // I32 a_dx = (px_abs - chunkPositionX_abs);
    // I32 a_dy = (py_abs - chunkPositionY_abs);
    // I32 a_dz = (pz_abs - chunkPositionZ_abs);
    // I32 b_dx = (chunkPositionX_abs - px_abs);
    // I32 b_dy = (chunkPositionY_abs - py_abs);
    // I32 b_dz = (chunkPositionZ_abs - pz_abs);
    // I8 bx = a_dx < b_dx;
    // I8 by = a_dy < b_dy;
    // I8 bz = a_dz < b_dz;

    // if (a_dx != 0) {
    //     I32 minX = bx ? newbbminX : MAX(newbbminX, oldbbmaxX);
    //     I32 maxX = bx ? MIN(newbbmaxX, oldbbminX) : newbbmaxX;

    //     minX -= 16;
    //     maxX += 16;
    //     dbGetChunksInRegion(client, minX, maxX, newbbminY, newbbmaxY, newbbminZ, newbbmaxZ);
    // }

    // if (a_dy != 0) {
    //     I32 minY = by ? newbbminY : MAX(newbbminY, oldbbmaxY);
    //     I32 maxY = by ? MIN(newbbmaxY, oldbbminY) : newbbmaxY;

    //     minY -= 16;
    //     maxY += 16;
    //     dbGetChunksInRegion(client, newbbminX, newbbmaxX, minY, maxY, newbbminZ, newbbmaxZ);
    // }

    // if (a_dz != 0) {
    //     I32 minZ = bz ? newbbminZ : MAX(newbbminZ, oldbbmaxZ);
    //     I32 maxZ = bz ? MIN(newbbmaxZ, oldbbminZ) : newbbmaxZ;

    //     minZ -= 16;
    //     maxZ += 16;
    //     dbGetChunksInRegion(client, newbbminX, newbbmaxX, newbbminY, newbbmaxY, minZ, maxZ);
    // }

    // logD("%d %d %d", a_dx, a_dy, a_dz);
    // logD("chunkPosition.x %d", client->chunkPosition.x);
    // logD("newbbminmaxX %d %d", newbbminX, newbbmaxX);
    // logD("newbbminmaxY %d %d", newbbminY, newbbmaxY);
    // logD("newbbminmaxZ %d %d", newbbminZ, newbbmaxZ);

    // int volume = (maxX - minX) * (maxY - minY) * (maxZ - minZ);
    // logD("minmax: %d %d %d %d %d %d", minX, maxX, minY, maxY, minZ, maxZ);
    // logD("diff: %d %d %d", dx, dy, dz);
    // logD("volume: %d", volume);

    vec(VECTORI) chunksToGet;
    init(&chunksToGet);

    for (I32 x = px - rd; x <= px + rd; x += CHUNK_SIZE) {
    for (I32 y = py - rd; y <= py + rd; y += CHUNK_SIZE) {
    for (I32 z = pz - rd; z <= pz + rd; z += CHUNK_SIZE) {
        if (worldGetChunk(client, x, y, z) == 0) {
            push(&chunksToGet, ((VECTORI){x, y, z}));
        }
    }}}


    CHUNK** chunksToAdd = malloc(sizeof(CHUNK*) * CUBE(2 * client->renderDistance));
    U32 addIndex = 0;

    // dbGetChunksInRegion(client, newbbminX, newbbmaxX, newbbminY, newbbmaxY, newbbminZ, newbbmaxZ);
    dbGetChunksInRegion(client, &chunksToGet);

    logD("size client->dbChunks: %d", size(&client->dbChunks));

    // perfTimerBegin("worldUpdateClientChunk");

    // clear(&vector);
    cleanup(&chunksToGet);


    for (I32 x = px - rd; x <= px + rd; x += CHUNK_SIZE) {
    for (I32 y = py - rd; y <= py + rd; y += CHUNK_SIZE) {
    for (I32 z = pz - rd; z <= pz + rd; z += CHUNK_SIZE) {
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

    if (addIndex) {
        dbAddChunks(chunksToAdd, addIndex);
    }
    free(chunksToAdd);
    // perfTimerEnd();
}
