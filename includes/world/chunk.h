#pragma once

#include "utils/types.h"
#include "utils/vector.h"
#include "utils/math.h"

#define CHUNK_SIZE 16
#define CHUNK_BLOCK_COUNT CUBE(CHUNK_SIZE)
#define TO_CHUNK_POS(x) ((x < 0 ? (x + 1) / CHUNK_SIZE - 1 : x / CHUNK_SIZE) * CHUNK_SIZE)

typedef struct chunk CHUNK;

struct chunk {
    U8 monotype;
    VECTORI position;
    U8* blocks;
};


typedef struct chunkHashmap CHUNK_HASHMAP;

struct chunkHashmap {
    VECTORI position;
    CHUNK* chunk;
    U8 used;
};

U8* chunkGenerate(I32 x, I32 y, I32 z);
U8 chunkIsMonotype(CHUNK* chunk);
U8 chunkIsEmpty(CHUNK* chunk);
U32 chunkHash(I32 x, I32 y, I32 z);
void chunkClean(CHUNK* chunk);
CHUNK* chunkCreate(I32 x, I32 y, I32 z);
