#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <thread>

#include <FastNoise/FastNoise.h>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include "glm/gtx/hash.hpp"
#include "glm/gtc/random.hpp"
#include "main.hpp"

#define CHUNK_SIZE (16)
#define CHUNK_BLOCK_COUNT (CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE)
#define INDEX_XYZ(x, y, z) ((z) * CHUNK_SIZE*CHUNK_SIZE + (y) * CHUNK_SIZE + (x))
#define IS_INSIDE_CHUNK(x, y, z) (((x) >= 0 && (x) < CHUNK_SIZE && (y) >= 0 && (y) < CHUNK_SIZE && (z) >= 0 && (z) < CHUNK_SIZE))

static FastNoise::SmartNode fn;

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

float map(float value, float min1, float max1, float min2, float max2) {
    return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

static void setBlock(uint8_t* blocks, uint8_t value, int localX, int localY, int localZ) {
    if (!IS_INSIDE_CHUNK(localX, localY, localZ)) return;
    int index = INDEX_XYZ(localX, localY, localZ);
    blocks[index] = value;
}

static void generateStage1(uint8_t* blocks, const glm::ivec3& chunkWorldPosition)
{
    float hmap[16*16];
    fn->GenUniformGrid2D(hmap, chunkWorldPosition.x, chunkWorldPosition.z, 16, 16, 0.01f, 0);

    for (int dz = 0 ; dz < CHUNK_SIZE ; ++dz) {
    for (int dy = 0 ; dy < CHUNK_SIZE ; ++dy) {
    for (int dx = 0 ; dx < CHUNK_SIZE ; ++dx) {
        int i = INDEX_XYZ(dx, dy, dz);

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
    const int32_t TREE_BOTTOM_CENTER_X = 0;//2;
    const int32_t TREE_BOTTOM_CENTER_Z = 0;//2;

    float random_X = fn->GenSingle3D(chunkWorldPosition.x, chunkWorldPosition.y, chunkWorldPosition.z, 0);
    float random_Z = fn->GenSingle3D(chunkWorldPosition.x, chunkWorldPosition.y, chunkWorldPosition.z, 1);

    int TREE_X = (random_X * 0.5f + 0.5f) * (CHUNK_SIZE-1 - TREE_BOTTOM_CENTER_X);
    int TREE_Z = (random_Z * 0.5f + 0.5f) * (CHUNK_SIZE-1 - TREE_BOTTOM_CENTER_Z);

    for (int32_t dy = CHUNK_SIZE - 2 ; dy >= 0 ; --dy) {
        int prev_index = INDEX_XYZ(TREE_X+TREE_BOTTOM_CENTER_X, dy+1, TREE_Z+TREE_BOTTOM_CENTER_Z);
        int index      = INDEX_XYZ(TREE_X+TREE_BOTTOM_CENTER_X, dy  , TREE_Z+TREE_BOTTOM_CENTER_Z);

        // can only grow on grass
        if (blocks[index] == 1 && blocks[prev_index] == 0) {
            return {TREE_X, dy + 1, TREE_Z};
        }
    }

    return {-1, -1, -1};
}

static void placeTemplate(uint8_t* blocks, glm::ivec3 localPos) {
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

        blocks[INDEX_XYZ(x, y, z)] = value;
    }}}
}

static void placeTree(uint8_t* blocks, const glm::ivec3& localPos, const glm::ivec3& chunkWorldPosition) {
    glm::ivec3 blockWorldPos = chunkWorldPosition + localPos;
    srand(blockWorldPos.x ^ blockWorldPos.y ^ blockWorldPos.z);

    int leavesBottom = glm::linearRand(3, 4);
    int height = glm::linearRand(8, 30);
    int leavesWidth = height / 4;
    int trunkWidth = 1;

    // Trunk
    const int trunkWidthMin = -(trunkWidth/2);
    const int trunkWidthMax = trunkWidth-(trunkWidth/2);
    const float trunkRadius = (float)trunkWidth * 0.5f;
    for (int y = 0 ; y < height ; ++y) {
        for (int x = trunkWidthMin ; x < trunkWidthMax ; ++x) {
        for (int z = trunkWidthMin ; z < trunkWidthMax ; ++z) {

            float offset = trunkWidth % 2 == 0 ? 0.5f : 0.0f;
            float sqDistFromTrunk = glm::length2(glm::vec2(x, z) + offset);
            if (sqDistFromTrunk > trunkRadius*trunkRadius) continue; // -1.5 for better roundness

            setBlock(blocks, 4, localPos.x + x, localPos.y + y, localPos.z + z);

        }}
    }

    // Leaves
    for (int y = leavesBottom ; y < height ; ++y) {
        // Add +1 to leaveWidth because the 0.01 messes the starting width
        // 0.01 to avoid bottom layer to be always one block wider
        int width = glm::mix(leavesWidth+1, 1, map(y, leavesBottom, height, 0.01f, 1.0f));

        for (int z = trunkWidthMin - width ; z < trunkWidthMax + width ; ++z) {
        for (int x = trunkWidthMin - width ; x < trunkWidthMax + width ; ++x) {
            if (y < height && x >= trunkWidthMin && x < trunkWidthMax && z >= trunkWidthMin && z < trunkWidthMax) continue;

            if (glm::abs(x) + glm::abs(z) == 1) { // block next to trunk
                setBlock(blocks, 5, localPos.x + x, localPos.y + y, localPos.z + z);
                continue;
            }

            float sqDistFromTrunk = glm::length2(glm::vec2(x, z));
            if (sqDistFromTrunk - 0.5 > width*width + 0.5) continue; // -1.5 for better roundness
            // if (sqDistFromTrunk - trunkRadius*trunkRadius - 0.5 > width*width + trunkWidth*trunkWidth + 0.5) continue; // -1.5 for better roundness

            const int decayChance = 4;
            if (rand() % 10 < sqDistFromTrunk - (width*width) + decayChance) continue;

            setBlock(blocks, 5, localPos.x + x, localPos.y + y, localPos.z + z);
        }}
    }

    // Top
    setBlock(blocks, 5, localPos.x, localPos.y + height, localPos.z);
}

static void generateTrees(uint8_t* blocks, const glm::ivec3& chunkWorldPosition) {
    // LOW HIGH depends on feature/tree bounding box, need to be adjusted if bigger than CHUNK_SIZE
    #define LOW_X -1
    #define HIGH_X 1
    #define LOW_Y -2
    #define HIGH_Y 0
    #define LOW_Z -1
    #define HIGH_Z 1

    for (int chunk_z = LOW_X ; chunk_z <= HIGH_X ; ++chunk_z) {
    for (int chunk_y = LOW_Y ; chunk_y <= HIGH_Y ; ++chunk_y) {
    for (int chunk_x = LOW_Z ; chunk_x <= HIGH_Z ; ++chunk_x) {

        glm::ivec3 chunkOffset = glm::ivec3(chunk_x, chunk_y, chunk_z) * CHUNK_SIZE;
        glm::ivec3 other_chunkWorldPosition = chunkWorldPosition + chunkOffset;

        std::vector<glm::ivec3> spawnPoints;

        if (chunk_x == 0 && chunk_y == 0 && chunk_z == 0) {
            glm::ivec3 spawn_point = findTreeSpawnpoint(blocks, other_chunkWorldPosition);
            if (spawn_point.x != -1) spawnPoints.push_back(spawn_point);
        } else {
            generateStage1(temp_chunk, other_chunkWorldPosition);
            glm::ivec3 spawn_point = findTreeSpawnpoint(temp_chunk, other_chunkWorldPosition);
            if (spawn_point.x != -1) spawnPoints.push_back(spawn_point + chunkOffset);
        }

        for (const auto& spawnpoint : spawnPoints) {
            placeTree(blocks, spawnpoint, chunkWorldPosition);
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
