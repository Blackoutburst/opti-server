#include "utils/math.h"
#include "world/chunk.h"


U32 xyzToIndex(I32 x, I32 y, I32 z) {
    return x + CHUNK_SIZE * (y + CHUNK_SIZE * z);
}

void indexToXYZ(VECTORI* vector, I16 index) {
    vector->x = index % CHUNK_SIZE;
    vector->y = (index / CHUNK_SIZE) % CHUNK_SIZE;
    vector->z = (index / (CHUNK_SIZE * CHUNK_SIZE)) % CHUNK_SIZE;
}
