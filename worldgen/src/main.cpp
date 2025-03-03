#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <thread>

#define CHUNK_SIZE (16)
#define CHUNK_BLOCK_COUNT (CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE)

#include "FastNoise/FastNoise.h"

#define GLM_FORCE_SWIZZLE
#include "glm/glm.hpp"

#include "main.hpp"

thread_local uint8_t temp_chunk[CHUNK_BLOCK_COUNT];

const uint32_t TREE_X_SIZE = 5;
const uint32_t TREE_Y_SIZE = 5;
const uint32_t TREE_Z_SIZE = 5;

const uint8_t tree[] = {
    0, 0, 0, 0, 0,
    0, 5, 5, 5, 0,
    0, 5, 5, 5, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,

    0, 5, 5, 5, 0,
    5, 5, 5, 5, 5,
    5, 5, 5, 5, 5,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,

    0, 5, 5, 5, 0,
    5, 5, 4, 5, 5,
    5, 5, 4, 5, 5,
    0, 0, 4, 0, 0,
    0, 0, 4, 0, 0,

    0, 5, 5, 5, 0,
    5, 5, 5, 5, 5,
    5, 5, 5, 5, 5,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,

    0, 0, 0, 0, 0,
    0, 5, 5, 5, 0,
    0, 5, 5, 5, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0
};


#define IS_INSIDE_CHUNK(x, y, z) ((x >= 0 && x < CHUNK_SIZE && y >= 0 && y < CHUNK_SIZE && z >= 0 && z < CHUNK_SIZE))


void generateStage1(uint8_t* blocks, int32_t x, int32_t y, int32_t z)
{
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

        float height = (2 + hmap[iXZ]) * 30.0f;

        if (world_y >= height) {
            blocks[i] = 0;
        } else {
            blocks[i] = 1;
        }
    }}}
}


// xyz -> chunk world position
glm::ivec3 findTreeSpawnpoint(uint8_t* blocks, int32_t x, int32_t y, int32_t z) {
    const int32_t TREE_X = 0;// (CHUNK_SIZE / 2);
    const int32_t TREE_Z = 0;//(CHUNK_SIZE / 2);
    const int32_t TREE_BOTTOM_CENTER_X = 2;//(CHUNK_SIZE / 2);
    const int32_t TREE_BOTTOM_CENTER_Z = 2;//(CHUNK_SIZE / 2);

    for (int32_t dy = CHUNK_SIZE - 2 ; dy >= 0 ; --dy) {
        int prev_index = (TREE_Z+TREE_BOTTOM_CENTER_Z) * CHUNK_SIZE*CHUNK_SIZE + (dy+1)*CHUNK_SIZE + (TREE_X+TREE_BOTTOM_CENTER_X);
        int index      = (TREE_Z+TREE_BOTTOM_CENTER_Z) * CHUNK_SIZE*CHUNK_SIZE +  dy*CHUNK_SIZE    + (TREE_X+TREE_BOTTOM_CENTER_X);

        if (blocks[index] > 0 && blocks[prev_index] == 0) {
            return {TREE_BOTTOM_CENTER_X, dy + 1, TREE_BOTTOM_CENTER_Z};
        }
    }
}

void placeTree(uint8_t* blocks, int32_t origin_x, int32_t origin_y, int32_t origin_z) {
    for (int32_t dz = 0 ; dz < TREE_Z_SIZE ; ++dz) {
    for (int32_t dy = 0 ; dy < TREE_Y_SIZE ; ++dy) {
    for (int32_t dx = 0 ; dx < TREE_X_SIZE ; ++dx) {
        int32_t value_index = (dz*TREE_Z_SIZE*TREE_Z_SIZE) + ((TREE_Y_SIZE-1-dy)*TREE_Y_SIZE) + dx;
        uint8_t value = tree[value_index];

        if (value == 0) continue;

        int32_t x = origin_x + dx;
        int32_t y = origin_y + dy;
        int32_t z = origin_z + dz;

        if (!IS_INSIDE_CHUNK(x, y, z)) continue;

        const int32_t blocks_index = (z*CHUNK_SIZE*CHUNK_SIZE) + (y*CHUNK_SIZE) + x;
        blocks[blocks_index] = value;
    }}}
}

void generateTrees(uint8_t* blocks, int chunk_offset_x, int chunk_offset_y, int chunk_offset_z) {
    for (int chunk_z = -1 ; chunk_z <= 1 ; ++chunk_z) {
    for (int chunk_y = -1 ; chunk_y <= 1 ; ++chunk_y) {
    for (int chunk_x = -1 ; chunk_x <= 1 ; ++chunk_x) {

        // if (chunk_x == 0 && chunk_y == 0 && chunk_z == 0) {
        //     glm::ivec3 spawn_point = findTreeSpawnpoint();

        //     generateTrees(blocks, chunk_x * CHUNK_SIZE, chunk_y * CHUNK_SIZE, chunk_z * CHUNK_SIZE);
        // } else {
        //     generateStage1(temp_chunk, chunk_offset_x, chunk_offset_y, chunk_offset_z);
        //     findTreeSpawnpoint();
        //     // generateTrees(blocks, chunk_x * CHUNK_SIZE, chunk_y * CHUNK_SIZE, chunk_z * CHUNK_SIZE);
        // }

    }}}

        // if (blocks[index] > 0 && blocks[prev_index] == 0) {
        //     placeTree(blocks, chunk_offset_x + 0, chunk_offset_y + dy+1, chunk_offset_z + 0);
        //     // placeTree(blocks, TREE_X, dy+1, TREE_Z);
        //     break;
        // }
}

void genChunk(uint8_t* blocks, int32_t x, int32_t y, int32_t z) {
    generateStage1(blocks, x, y, z);

    for (int chunk_z = -1 ; chunk_z <= 1 ; ++chunk_z) {
    for (int chunk_y = -1 ; chunk_y <= 1 ; ++chunk_y) {
    for (int chunk_x = -1 ; chunk_x <= 1 ; ++chunk_x) {
        // if (chunk_x == 0 && chunk_y == 0 && chunk_z == 0) continue;
        // If neighbour as tree, generate it too

        generateTrees(blocks, chunk_x * CHUNK_SIZE, chunk_y * CHUNK_SIZE, chunk_z * CHUNK_SIZE);
    }}}
}
