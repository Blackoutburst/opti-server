#include <stdio.h>
#include <stdint.h>
#include <iostream>

#define CHUNK_SIZE (16)

#include "FastNoise/FastNoise.h"
// #include "main.h"

extern "C" void genChunk(uint8_t* blocks, int32_t x, int32_t y, int32_t z) {
    // auto fn = FastNoise::New<FastNoise::Perlin>();

    float hmap[4096] = {};
    // fn->GenUniformGrid2D(hmap, 0, 0, 16, 16, 4, 0);

    for (int dz = 0 ; dz < CHUNK_SIZE ; ++dz) {
    for (int dy = 0 ; dy < CHUNK_SIZE ; ++dy) {
    for (int dx = 0 ; dx < CHUNK_SIZE ; ++dx) {
        int i = dz * CHUNK_SIZE*CHUNK_SIZE + dy * CHUNK_SIZE + dx;
        int iXZ = dz * CHUNK_SIZE + dx;

        int world_y = y + dy;

        if (world_y >= hmap[iXZ] * 10.0f) {
            blocks[i] = 0;
        } else {
            blocks[i] = 2;
        }
    }
    }
    }
}


int main()
{
    auto fn = FastNoise::New<FastNoise::Perlin>();

    std::cout << "Salut" << std::endl;
    printf("HELLO CA FONCTIONNE !!\n");

    return 0;
}
