#pragma once

#include "utils/types.h"
#include "utils/vector.h"

#define CHUNK_SIZE 16
#define CHUNK_BLOCK_COUNT CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE

typedef struct chunk CHUNK;

struct chunk {
    VECTORI position;
    U8* blocks;
};

U8 chunkIsMonotype(CHUNK* chunk);
U8 chunkIsEmpty(CHUNK* chunk);
U32 chunkHash(I32 x, I32 y, I32 z);
void chunkClean(CHUNK* chunk);
CHUNK* chunkCreate(I32 x, I32 y, I32 z);
