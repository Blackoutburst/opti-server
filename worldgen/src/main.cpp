#include <stdio.h>
#include <stdint.h>
#include <iostream>

#define CHUNK_SIZE (16)

#include "FastNoise/FastNoise.h"

#define GLM_FORCE_SWIZZLE
#include "glm/glm.hpp"

#include "main.h"

// float de(glm::vec3 p)
// {
//     glm::vec2 t = glm::fract(glm::vec2(p.x, p.z))-0.5f;
//     p.x=t.x;
//     p.z=t.y;
//     float k=1.0f;
//     float s=0.0f;
//     for(int i =0;i++<9;)
//       s=2./glm::clamp(glm::dot(p,p),0.1f,1.0f),
//       p=glm::abs(p)*s-glm::vec3(0.5f, 3.0f, 0.5f),
//       k*=s;
//     return glm::length(p)/k-.001f;
//   }

// float de(glm::vec3 p){  //(distance bound doesn't hold)
//     glm::vec3 v=glm::vec3(0,1.5,6.3);
//     return glm::min(6.0f-glm::length((p-v).xy+glm::sin(p.yx())), glm::dot(glm::cos(p),glm::sin(p.yzx())))+glm::sin(glm::sin(p.z*3.5f)+v.z)*0.1f+1.0f;
// }


// if ( de(glm::vec3(world_x, world_y - 200, world_z) * 0.01f) < 0.005f) {
float de(glm::vec3 p) {
    constexpr float SCALE = 0.0025f;

    if (!(p.x > -1.0f/SCALE && p.x > -1.0f/SCALE && p.y < 0.6f/SCALE && p.y < 0.8f/SCALE && p.z > -0.62f/SCALE && p.z < 1.2f/SCALE)) return 1.0f;
    p *= SCALE;

    const glm::vec3 va = glm::vec3(  0.0,  0.57735,  0.0 );
    const glm::vec3 vb = glm::vec3(  0.0, -1.0,  1.15470 );
    const glm::vec3 vc = glm::vec3(  1.0, -1.0, -0.57735 );
    const glm::vec3 vd = glm::vec3( -1.0, -1.0, -0.57735 );
    float a = 0.0f;
    float s = 1.0f;
    float r = 1.0f;
    float dm;
    glm::vec3 v;

    for(int i=0; i<16; i++) {
        float d, t;
        d = glm::dot(p-va,p-va);              v=va; dm=d; t=0.0f;
        d = glm::dot(p-vb,p-vb); if( d < dm ) { v=vb; dm=d; t=1.0f; }
        d = glm::dot(p-vc,p-vc); if( d < dm ) { v=vc; dm=d; t=2.0f; }
        d = glm::dot(p-vd,p-vd); if( d < dm ) { v=vd; dm=d; t=3.0f; }
        p = v + 2.0f*(p - v); r*= 2.0f;
        a = t + 4.0f*a; s*= 4.0f;
    }
    return (glm::sqrt(dm)-1.0f)/r;
  }


void genChunk(uint8_t* blocks, int32_t x, int32_t y, int32_t z) {
    auto fn = FastNoise::New<FastNoise::Perlin>();

    float hmap[16*16];
    fn->GenUniformGrid2D(hmap, x, z, 16, 16, 0.01f, 0);

    for (int dz = 0 ; dz < CHUNK_SIZE ; ++dz) {
    for (int dy = 0 ; dy < CHUNK_SIZE ; ++dy) {
    for (int dx = 0 ; dx < CHUNK_SIZE ; ++dx) {
        int i = dz * CHUNK_SIZE*CHUNK_SIZE + dy * CHUNK_SIZE + dx;
        int iXZ = dz * CHUNK_SIZE + dx;

        int world_x = x + dx;
        int world_y = y + dy;
        int world_z = z + dz;

        // if ( de(glm::vec3(world_x, world_y - 450, world_z)) < 0.005f) {
        //     blocks[i] = 3;
        //     continue;
        // }

        if (world_y >= (2 + hmap[iXZ]) * 30.0f) {
            blocks[i] = 0;
        } else {
            blocks[i] = 1;
        }
    }
    }
    }
}
