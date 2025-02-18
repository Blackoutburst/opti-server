#include <stdlib.h>
#include <stdio.h>
#include "utils/math.h"
#include "world/world.h"
#include "network/client.h"
#include "utils/math.h"
#include "database/database.h"
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
    I32 rd = ((I32)client->renderDistance) * CHUNK_SIZE;

    logD("chunkPosition %d %d %d", client->chunkPosition.x, client->chunkPosition.y, client->chunkPosition.z);

    // I32 minX = px - rd;
    // I32 maxX = px + rd;
    // I32 minY = py - rd;
    // I32 maxY = py + rd;
    // I32 minZ = pz - rd;
    // I32 maxZ = pz + rd;


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

    I32 px_abs = (px);
    I32 py_abs = (py);
    I32 pz_abs = (pz);
    I32 chunkPositionX_abs = (client->chunkPosition.x);
    I32 chunkPositionY_abs = (client->chunkPosition.y);
    I32 chunkPositionZ_abs = (client->chunkPosition.z);
    I32 a_dx = (px_abs - chunkPositionX_abs);
    I32 a_dy = (py_abs - chunkPositionY_abs);
    I32 a_dz = (pz_abs - chunkPositionZ_abs);
    I32 b_dx = (chunkPositionX_abs - px_abs);
    I32 b_dy = (chunkPositionY_abs - py_abs);
    I32 b_dz = (chunkPositionZ_abs - pz_abs);
    I8 bx = a_dx < b_dx;
    I8 by = a_dy < b_dy;
    I8 bz = a_dz < b_dz;

    logD("p_abs: %d %d %d", px_abs, py_abs, pz_abs);
    logD("chunkPosition_abs: %d %d %d", chunkPositionX_abs, chunkPositionY_abs, chunkPositionZ_abs);
    logD("a_d: %d %d %d", a_dx, a_dy, a_dz);
    logD("b_d: %d %d %d", b_dx, b_dy, b_dz);

    I32 bothminX = MIN(oldbbminX, newbbminX);
    I32 bothmaxX = MAX(oldbbmaxX, newbbmaxX);

    I32 bothminY = MIN(oldbbminY, newbbminY);
    I32 bothmaxY = MAX(oldbbmaxY, newbbmaxY);

    I32 bothminZ = MIN(oldbbminZ, newbbminZ);
    I32 bothmaxZ = MAX(oldbbmaxZ, newbbmaxZ);

    // bothminX -= CHUNK_SIZE;
    // bothmaxX += CHUNK_SIZE;

    // bothminY -= CHUNK_SIZE;
    // bothmaxY += CHUNK_SIZE;

    // bothminZ -= CHUNK_SIZE;
    // bothmaxZ += CHUNK_SIZE;

    // if (a_dx != 0) {
        I32 minX = bx ? newbbminX : MAX(newbbminX, oldbbmaxX);
        I32 maxX = bx ? MIN(newbbmaxX, oldbbminX) : newbbmaxX;

        // minX -= CHUNK_SIZE;
        // maxX += CHUNK_SIZE;
        dbGetChunksInRegion(client, minX, maxX, bothminY, bothmaxY, bothminZ, bothmaxZ);
    // }

    // if (a_dy != 0) {
        I32 minY = by ? newbbminY : MAX(newbbminY, oldbbmaxY);
        I32 maxY = by ? MIN(newbbmaxY, oldbbminY) : newbbmaxY;

        // minY -= CHUNK_SIZE;
        // maxY += CHUNK_SIZE;
        dbGetChunksInRegion(client, bothminX, bothmaxX, minY, maxY, bothminZ, bothmaxZ);
    // }

    // if (a_dz != 0) {
        I32 minZ = bz ? newbbminZ : MAX(newbbminZ, oldbbmaxZ);
        I32 maxZ = bz ? MIN(newbbmaxZ, oldbbminZ) : newbbmaxZ;

        // minZ -= CHUNK_SIZE;
        // maxZ += CHUNK_SIZE;
        dbGetChunksInRegion(client, bothminX, bothmaxX, bothminY, bothmaxY, minZ, maxZ);
    // }


    // dbGetChunksInRegion(client, newbbminX, newbbmaxX, newbbminY, newbbmaxY, newbbminZ, newbbmaxZ);
    // dbGetChunksInRegion(client, bothminX, bothmaxX, bothminY, bothmaxY, bothminZ, bothmaxZ);

    // logD("%d %d %d", a_dx, a_dy, a_dz);
    logD("b: %d %d %d", bx, by, bz);

    logD("minX, maxX: %d %d", minX, maxX);
    logD("minY, maxY: %d %d", minY, maxY);
    logD("minZ, maxZ: %d %d", minZ, maxZ);
    // logD("chunkPosition.x %d", client->chunkPosition.x);

    // logD("newbbminmaxX %d %d", newbbminX, newbbmaxX);
    // logD("newbbminmaxY %d %d", newbbminY, newbbmaxY);
    // logD("newbbminmaxZ %d %d", newbbminZ, newbbmaxZ);

    // logD("bothbbminmaxX %d %d", bothminX, bothmaxX);
    // logD("bothbbminmaxY %d %d", bothminY, bothmaxY);
    // logD("bothbbminmaxZ %d %d", bothminZ, bothmaxZ);

    logD("");
    // int volume = (maxX - minX) * (maxY - minY) * (maxZ - minZ);
    // logD("minmax: %d %d %d %d %d %d", minX, maxX, minY, maxY, minZ, maxZ);
    // logD("diff: %d %d %d", dx, dy, dz);
    // logD("volume: %d", volume);

    CHUNK** chunksToAdd = malloc(sizeof(CHUNK*) * CUBE(2 * client->renderDistance));
    U32 addIndex = 0;

    // dbGetChunksInRegion(client, minX, maxX, minY, maxY, minZ, maxZ);

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

    if (addIndex) dbAddChunks(chunksToAdd, addIndex);
    free(chunksToAdd);
}
