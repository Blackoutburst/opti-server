#include <stdlib.h>
#include <string.h>
#include "world/chunk.h"
#include "library/library.h"

static worldgen_genChunk func_genChunk = NULL;

void chunkSetGenChunkFunction(worldgen_genChunk func) {
    func_genChunk = func;
}

U8* chunkGenerate(I32 x, I32 y, I32 z) {
    U8* blocks = malloc(sizeof(U8) * CHUNK_BLOCK_COUNT);

    if (func_genChunk != NULL) {
       func_genChunk(blocks, x, y, z);
    } else {
        for (U32 i = 0; i < CHUNK_BLOCK_COUNT; i++) {
            I32 by = (i / CHUNK_SIZE) % CHUNK_SIZE;
            blocks[i] = y + by < 0;
        }
    }

    return blocks;
}

U8 _chunkIsMonotype(U8* blocks) {
    I32 previous = blocks[0];

    for (I32 i = 1; i < CHUNK_BLOCK_COUNT; i++) {
        if (previous != blocks[i])
            return 0;
        previous = blocks[i];
    }

    return 1;
}

U8 chunkIsMonotype(CHUNK* chunk) {
    if (chunk == NULL) return 0;
    if (chunk->monotype) return 1;

    return _chunkIsMonotype(chunk->blocks);
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

CHUNK* chunkAssemble(I32 x, I32 y, I32 z, U8* blocks) {
    CHUNK* chunk = malloc(sizeof(CHUNK));
    chunk->position.x = x;
    chunk->position.y = y;
    chunk->position.z = z;

    chunk->monotype = _chunkIsMonotype(blocks);
    if (chunk->monotype) {
        chunk->blocks = malloc(sizeof(U8));
        chunk->blocks[0] = blocks[0];
        free(blocks);
    } else {
        chunk->blocks = blocks;
    }

    return chunk;
}

CHUNK* chunkCreate(I32 x, I32 y, I32 z) {
    CHUNK* chunk = malloc(sizeof(CHUNK));
    chunk->position.x = x;
    chunk->position.y = y;
    chunk->position.z = z;

    U8* blocks = chunkGenerate(x, y, z);
    chunk->monotype = _chunkIsMonotype(blocks);
    if (chunk->monotype) {
        chunk->blocks = malloc(sizeof(U8));
        chunk->blocks[0] = blocks[0];
        free(blocks);
    } else {
        chunk->blocks = blocks;
    }

    return chunk;
}
