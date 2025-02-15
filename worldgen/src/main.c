#include <stdio.h>
#include <stdint.h>

#define CHUNK_SIZE (16)

#include "FastNoise.h"

void genChunk(uint8_t* blocks, int32_t x, int32_t y, int32_t z) {
    for (int dz = 0 ; dz < CHUNK_SIZE ; ++dz) {
    for (int dy = 0 ; dy < CHUNK_SIZE ; ++dy) {
    for (int dx = 0 ; dx < CHUNK_SIZE ; ++dx) {
        int i = dz * CHUNK_SIZE*CHUNK_SIZE + dy * CHUNK_SIZE + dx;

        int world_y = y + dy;

        if (world_y >= 0) {
            blocks[i] = 0;
        } else {
            blocks[i] = 2;
        }
    }
    }
    }
}
