#include <stdlib.h>
#include <stdio.h>
#include "world/world.h"
#include "network/client.h"
#include "database/database.h"
#include "utils/math.h"
#include "utils/tpool.h"
#include "world/chunk.h"
#include "utils/perfTimer.h"
#include "utils/cpthread.h"
// #include "utils/logger.h"


// -- MULTITHREADING -- //
// TODO: put this stuff elsewhere to make it work for multiple players
static tpool_t* thread_pool = NULL;
static vec(CHUNK*) chunks_generated;
static pthread_mutex_t chunks_generated_mutex;

// typedef struct funcArg funcArg_t;
// struct funcArg {
//     I32 x;
//     I32 y;
//     I32 z;
//     U8* blocks;
// };

static void func(void* args) {
    int x = ((int*)args)[0];
    int y = ((int*)args)[1];
    int z = ((int*)args)[2];

    CHUNK* c = chunkCreate(x, y, z);

    pthread_mutex_lock(&chunks_generated_mutex);
    push(&chunks_generated, c);
    pthread_mutex_unlock(&chunks_generated_mutex);

    free(args);
}
// -- //


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

    static int a = 0;
    if (a == 0) {
        a = 1;
        thread_pool = tpool_create(16);
        init(&chunks_generated);
        pthread_mutex_init(&chunks_generated_mutex, NULL);
    }

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

    I32 dx = (px - client->chunkPosition.x);
    I32 dy = (py - client->chunkPosition.y);
    I32 dz = (pz - client->chunkPosition.z);

    // logD("chunkPosition %d %d %d", client->chunkPosition.x, client->chunkPosition.y, client->chunkPosition.z);
    // logD("%d %d %d", bx, by, bz);
    // logD("p: %d %d %d", px, py, pz);
    // logD("chunkPosition: %d %d %d", client->chunkPosition.x, client->chunkPosition.y, client->chunkPosition.z);
    // logD("a_d: %d %d %d", a_dx, a_dy, a_dz);

    // logD("newbbminmaxX %d %d", newbbminX, newbbmaxX);
    // logD("newbbminmaxY %d %d", newbbminY, newbbmaxY);
    // logD("newbbminmaxZ %d %d", newbbminZ, newbbmaxZ);

    if (dx != 0) {
        I32 minX = dx > 0 ? MAX(newbbminX, oldbbmaxX) : MIN(newbbminX, oldbbminX);
        I32 maxX = dx > 0 ? MAX(newbbmaxX, oldbbmaxX) : MIN(newbbmaxX, oldbbminX);

        // logD("minX: %d, maxX: %d - %d", minX, maxX, maxX - minX);
        // int volume = (maxX - minX) * (newbbmaxY - newbbminY) * (newbbmaxZ - newbbminZ);
        // logD("Volume X: %d", volume / 4096);

        dbGetChunksInRegion(client, minX, maxX, newbbminY, newbbmaxY, newbbminZ, newbbmaxZ);
    }

    if (dy != 0) {
        I32 minY = dy > 0 ? MAX(newbbminY, oldbbmaxY) : MIN(newbbminY, oldbbminY);
        I32 maxY = dy > 0 ? MAX(newbbmaxY, oldbbmaxY) : MIN(newbbmaxY, oldbbminY);

        // logD("minY: %d, maxY: %d - %d", minY, maxY, maxY - minY);
        // int volume = (newbbmaxX - newbbminX) * (maxY - minY) * (newbbmaxZ - newbbminZ);
        // logD("Volume Y: %d", volume / 4096);

        dbGetChunksInRegion(client, newbbminX, newbbmaxX, minY, maxY, newbbminZ, newbbmaxZ);
    }

    if (dz != 0) {
        I32 minZ = dz > 0 ? MAX(newbbminZ, oldbbmaxZ) : MIN(newbbminZ, oldbbminZ);
        I32 maxZ = dz > 0 ? MAX(newbbmaxZ, oldbbmaxZ) : MIN(newbbmaxZ, oldbbminZ);

        // logD("minZ: %d, maxZ: %d - %d", minZ, maxZ, maxZ - minZ);
        // int volume = (newbbmaxX - newbbminX) * (newbbmaxY - newbbminY) * (maxZ - minZ);
        // logD("Volume Z: %d", volume / 4096);

        dbGetChunksInRegion(client, newbbminX, newbbmaxX, newbbminY, newbbmaxY, minZ, maxZ);
    }

    CHUNK** chunksToAdd = malloc(sizeof(CHUNK*) * CUBE(2 * client->renderDistance));
    U32 addIndex = 0;

    perfTimerBegin("worldUpdateClientChunk");

    // Do not use <= or it will not match the above code
    for (I32 x = px - rd; x < px + rd; x += CHUNK_SIZE) {
    for (I32 y = py - rd; y < py + rd; y += CHUNK_SIZE) {
    for (I32 z = pz - rd; z < pz + rd; z += CHUNK_SIZE) {
        U8** data = get(&client->dbChunks, ((VECTORI){x, y, z}));
        if (y > 256 || y < -256 || worldGetChunk(client, x, y, z)) {
            if (data != NULL) free(*data);
            continue;
        }

        CHUNK* c = NULL;
        if (data == NULL) {
            // c = chunkCreate(x, y, z);
            // chunksToAdd[addIndex++] = c;
            int* p = malloc(3 * sizeof(int));
            p[0] = x;
            p[1] = y;
            p[2] = z;
            tpool_add_work(thread_pool, func, p);
            continue;
        } else {
            c = chunkAssemble(x, y, z, *data);
        }

        worldAddChunk(client, c);

        if (!chunkIsEmpty(c)) {
            if (chunkIsMonotype(c)) {
                clientSendMonotypeChunk(client, c);
            } else {
                clientSendChunk(client, c);
            }
        }

        if (data != NULL) {  // Clean if chunk was already in db
            chunkClean(c);
        }
    }}}


    tpool_wait(thread_pool); // TODO: just lock the list instead of waiting
    for_each(&chunks_generated, c) {
        chunksToAdd[addIndex++] = *c;

        worldAddChunk(client, *c);

        if (!chunkIsEmpty(*c)) {
            if (chunkIsMonotype(*c)) {
                clientSendMonotypeChunk(client, *c);
            } else {
                clientSendChunk(client, *c);
            }
        }

    }
    clear(&chunks_generated);


    perfTimerEnd();

    clear(&client->dbChunks);

    if (addIndex) dbAddChunks(chunksToAdd, addIndex);
    free(chunksToAdd);
}
