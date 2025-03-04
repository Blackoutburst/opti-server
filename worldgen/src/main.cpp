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

static FastNoise::SmartNode fn;

static void generateStage1(uint8_t* blocks, const glm::ivec3& chunkWorldPosition)
{
    float hmap[16*16];
    fn->GenUniformGrid2D(hmap, chunkWorldPosition.x, chunkWorldPosition.z, 16, 16, 0.01f, 0);

    for (int dz = 0 ; dz < CHUNK_SIZE ; ++dz) {
    for (int dy = 0 ; dy < CHUNK_SIZE ; ++dy) {
    for (int dx = 0 ; dx < CHUNK_SIZE ; ++dx) {
        int i = dz * CHUNK_SIZE*CHUNK_SIZE + dy * CHUNK_SIZE + dx;
        int iXZ = dz * CHUNK_SIZE + dx;

        glm::ivec3 blockWorldPosition = chunkWorldPosition + glm::ivec3(dx, dy, dz);

        // if ( de(glm::vec3(world_x, world_y - 450, world_z)) < 0.005f) {
        //     blocks[i] = 3;
        //     continue;
        // }

        float height = (2 + hmap[iXZ]) * 15.0f;

        if (blockWorldPosition.y >= height) {
            blocks[i] = 0;
        } else {
            blocks[i] = 1;
        }
    }}}
}


// xyz -> chunk world position
static glm::ivec3 findTreeSpawnpoint(uint8_t* blocks, const glm::ivec3& chunkWorldPosition) {
    const int32_t TREE_BOTTOM_CENTER_X = 2;
    const int32_t TREE_BOTTOM_CENTER_Z = 2;

    float random_X = fn->GenSingle3D(chunkWorldPosition.x, chunkWorldPosition.y, chunkWorldPosition.z, 0);
    float random_Z = fn->GenSingle3D(chunkWorldPosition.x, chunkWorldPosition.y, chunkWorldPosition.z, 1);

    int TREE_X = (random_X * 0.5f + 0.5f) * (CHUNK_SIZE-1 - TREE_BOTTOM_CENTER_X);
    int TREE_Z = (random_Z * 0.5f + 0.5f) * (CHUNK_SIZE-1 - TREE_BOTTOM_CENTER_Z);

    for (int32_t dy = CHUNK_SIZE - 2 ; dy >= 0 ; --dy) {
        int prev_index = (TREE_Z+TREE_BOTTOM_CENTER_Z) * CHUNK_SIZE*CHUNK_SIZE + (dy+1)*CHUNK_SIZE + (TREE_X+TREE_BOTTOM_CENTER_X);
        int index      = (TREE_Z+TREE_BOTTOM_CENTER_Z) * CHUNK_SIZE*CHUNK_SIZE +  dy*CHUNK_SIZE    + (TREE_X+TREE_BOTTOM_CENTER_X);

        // can only grow on grass
        if (blocks[index] == 1 && blocks[prev_index] == 0) {
            return {TREE_X, dy + 1, TREE_Z};
        }
    }

    return {-1, -1, -1};
}

static void placeTree(uint8_t* blocks, glm::ivec3 localPos) {
    for (int32_t dz = 0 ; dz < TREE_Z_SIZE ; ++dz) {
    for (int32_t dy = 0 ; dy < TREE_Y_SIZE ; ++dy) {
    for (int32_t dx = 0 ; dx < TREE_X_SIZE ; ++dx) {
        int32_t value_index = (dz*TREE_Z_SIZE*TREE_Z_SIZE) + ((TREE_Y_SIZE-1-dy)*TREE_Y_SIZE) + dx;
        uint8_t value = tree[value_index];

        if (value == 0) continue;

        int32_t x = localPos.x + dx;
        int32_t y = localPos.y + dy;
        int32_t z = localPos.z + dz;

        if (!IS_INSIDE_CHUNK(x, y, z)) continue;

        const int32_t blocks_index = (z*CHUNK_SIZE*CHUNK_SIZE) + (y*CHUNK_SIZE) + x;
        blocks[blocks_index] = value;
    }}}
}

static void generateTrees(uint8_t* blocks, const glm::ivec3& chunkWorldPosition) {
    #define LOW_X -1
    #define HIGH_X 1
    #define LOW_Y -1
    #define HIGH_Y 0
    #define LOW_Z -1
    #define HIGH_Z 1

    for (int chunk_z = LOW_X ; chunk_z <= HIGH_X ; ++chunk_z) {
    for (int chunk_y = LOW_Y ; chunk_y <= HIGH_Y ; ++chunk_y) {
    for (int chunk_x = LOW_Z ; chunk_x <= HIGH_Z ; ++chunk_x) {

        // int chunk_offset_x = chunk_x * CHUNK_SIZE;
        // int chunk_offset_y = chunk_y * CHUNK_SIZE;
        // int chunk_offset_z = chunk_z * CHUNK_SIZE;

        glm::ivec3 chunkOffset = glm::ivec3(chunk_x, chunk_y, chunk_z) * CHUNK_SIZE;
        glm::ivec3 other_chunkWorldPosition = chunkWorldPosition + chunkOffset;

        if (chunk_x == 0 && chunk_y == 0 && chunk_z == 0) {
            glm::ivec3 spawn_point = findTreeSpawnpoint(blocks, other_chunkWorldPosition);

            if (spawn_point.x != -1) {
                placeTree(blocks, spawn_point);
            }
        } else {
            generateStage1(temp_chunk, other_chunkWorldPosition);
            glm::ivec3 spawn_point = findTreeSpawnpoint(temp_chunk, other_chunkWorldPosition);

            if (spawn_point.x != -1) {
                placeTree(blocks, spawn_point + chunkOffset);
            }
        }

    }}}
}

void init()
{
    // fn = FastNoise::New<FastNoise::Perlin>();
    fn = FastNoise::New<FastNoise::Simplex>();
}

void genChunk(uint8_t* blocks, int32_t x, int32_t y, int32_t z) {
    const glm::ivec3 chunkWorldPos = {x, y, z};

    generateStage1(blocks, chunkWorldPos);
    generateTrees(blocks, chunkWorldPos);
}
