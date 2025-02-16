#include <stdio.h>
#include <stdint.h>
#include <iostream>

#define CHUNK_SIZE (16)

#include "FastNoise/FastNoise.h"
#include "main.h"

// float fract(vec2 v) {
//     return x - floor(x)
// }

// float de(vec3 p){
//     p.xz=fract(p.xz)-.5;
//     float k=1.;
//     float s=0.;
//     for(int i=0;i++<9;)
//       s=2./clamp(dot(p,p),.1,1.),
//       p=abs(p)*s-vec3(.5,3,.5),
//       k*=s;
//     return length(p)/k-.001;
//   }

void genChunk(uint8_t* blocks, int32_t x, int32_t y, int32_t z) {
    auto fn = FastNoise::New<FastNoise::Perlin>();

    float hmap[16*16];
    fn->GenUniformGrid2D(hmap, x, z, 16, 16, 0.01f, 0);

    for (int dz = 0 ; dz < CHUNK_SIZE ; ++dz) {
    for (int dy = 0 ; dy < CHUNK_SIZE ; ++dy) {
    for (int dx = 0 ; dx < CHUNK_SIZE ; ++dx) {
        int i = dz * CHUNK_SIZE*CHUNK_SIZE + dy * CHUNK_SIZE + dx;
        int iXZ = dz * CHUNK_SIZE + dx;

        int world_y = y + dy;

        if (world_y >= (2 + hmap[iXZ]) * 30.0f) {
            blocks[i] = 0;
        } else {
            blocks[i] = 1;
        }
    }
    }
    }
}
