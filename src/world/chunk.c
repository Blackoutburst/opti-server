#include <stdlib.h>
#include "world/chunk.h"


U8 chunkIsMonotype(CHUNK* chunk) {
    if (chunk == NULL) return 0;

    I32 previous = chunk->blocks[0];

    for (I32 i = 1; i < CHUNK_BLOCK_COUNT; i++) {
        if (previous != chunk->blocks[i])
            return 0;
        previous = chunk->blocks[i];
    }

    return 1;
}

U8 chunkIsEmpty(CHUNK* chunk) {
    if (chunk == NULL) return 1;

    return chunkIsMonotype(chunk) && chunk->blocks[0] == 0;
}

U32 chunkHash(I32 x, I32 y, I32 z) {
    U32 h = 2166136261u;
    h = (h ^ (U32)x) * 16777619u;
    h = (h ^ (U32)y) * 16777619u;
    h = (h ^ (U32)z) * 16777619u;

    return h;
}

void chunkClean(CHUNK* chunk) {
    if (chunk == NULL) return;
    free(chunk->blocks);
    free(chunk);
}

CHUNK* chunkCreate(I32 x, I32 y, I32 z) {
    CHUNK* chunk = malloc(sizeof(CHUNK));
    chunk->position.x = x;
    chunk->position.y = y;
    chunk->position.z = z;
    chunk->blocks = malloc(sizeof(CHUNK_BLOCK_COUNT));
    for (U32 i = 0; i < CHUNK_BLOCK_COUNT; i++) chunk->blocks[i] = 0;

    return chunk;
}
