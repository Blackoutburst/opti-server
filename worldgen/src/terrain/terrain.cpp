#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include "glm/gtx/hash.hpp"
#include "glm/gtc/random.hpp"

#include "terrain/terrain.hpp"
#include "utils/easings.hpp"
#include "utils/grid.hpp"
#include "common.hpp"

void generateStage1(uint8_t* blocks, const glm::ivec3& chunkWorldPosition) {
    constexpr int SCALE = 2;
    constexpr int SIZE = (CHUNK_SIZE / SCALE) + 1;
    float hmap[SIZE*SIZE];
    fn->GenUniformGrid2D(hmap, chunkWorldPosition.x / SCALE, chunkWorldPosition.z / SCALE, SIZE, SIZE, 0.01f * SCALE, 0);

    // float hmap[16*16];
    // generateHeights(hmap, chunkWorldPosition);

    // float cmap[SIZE*SIZE];
    // fn_celullarValue->GenUniformGrid2D(cmap, chunkWorldPosition.x , chunkWorldPosition.z , SIZE, SIZE, 0.0025f * SCALE, 0);

    for (int dz = 0 ; dz < CHUNK_SIZE ; ++dz) {
    for (int dx = 0 ; dx < CHUNK_SIZE ; ++dx) {
        float rawHeight = getGridAtScaled2<SIZE, SCALE>(hmap, dx, dz);
        float height = (2 + rawHeight) * 15.0f;
        // height += getGridAtScaled2<SIZE, SCALE>(cmap, dx, dz) * 40.0f;

        for (int dy = 0 ; dy < CHUNK_SIZE ; ++dy) {
            const int i = INDEX_XYZ(dx, dy, dz);
            // int iXZ = dz * CHUNK_SIZE + dx;
            // float height = hmap[iXZ];
            // float height = (2 + hmap[iXZ]) * 15.0f;

            glm::ivec3 blockWorldPosition = chunkWorldPosition + glm::ivec3(dx, dy, dz);

            if (blockWorldPosition.y >= height) {
                blocks[i] = (uint8_t)BlockType::Air;
            } else if (blockWorldPosition.y >= height - 1.0f) {
                blocks[i] = (uint8_t)BlockType::Grass;
            } else if (blockWorldPosition.y >= height - 5.0f) {
                blocks[i] = (uint8_t)BlockType::Dirt;
            } else {
                blocks[i] = (uint8_t)BlockType::Stone;
            }
        }
    }}
}

// void generateStage1(uint8_t* blocks, const glm::ivec3& chunkWorldPosition) {
//     float terrainDenity[CHUNK_BLOCK_COUNT];
//     fn_terrain->GenUniformGrid3D(terrainDenity, chunkWorldPosition.x, chunkWorldPosition.y, chunkWorldPosition.z, 16, 16, 16, 0.004f, 1337);

//     float cmap[16*16];
//     fn_celullarValue->GenUniformGrid2D(cmap, chunkWorldPosition.x, chunkWorldPosition.z, 16, 16, 0.0025f, 0);

//     for (int dz = 0 ; dz < CHUNK_SIZE ; ++dz) {
//     for (int dy = 0 ; dy < CHUNK_SIZE ; ++dy) {
//     for (int dx = 0 ; dx < CHUNK_SIZE ; ++dx) {
//         int i = INDEX_XYZ(dx, dy, dz);

//         int iXZ = dz * CHUNK_SIZE + dx;

//         glm::ivec3 blockWorldPosition = chunkWorldPosition + glm::ivec3(dx, dy, dz);

//         float worldY = chunkWorldPosition.y + dy;
//         float density = terrainDenity[i];

//         float minFactor = -1.0f;// + mask * 0.1f;
//         float maxFactor = 1.0f;// + mask * 0.1f;
//         float heightFactor = glm::clamp(mapRange(worldY, -256, 256, minFactor, maxFactor), minFactor, maxFactor);
//         float threshold = heightFactor;

//         if (threshold > density) {
//             blocks[i] = (uint8_t)BlockType::Air;
//         } else if (threshold > density - 0.005f) {
//             blocks[i] = (uint8_t)BlockType::Grass;
//         } else if (threshold > density - 0.01f) {
//             blocks[i] = (uint8_t)BlockType::Dirt;
//         } else {
//             blocks[i] = (uint8_t)BlockType::Stone;
//         }
//     }}}
// }

void generateStage2(uint8_t* blocks, const glm::ivec3& chunkWorldPosition) {
    constexpr int SCALE = 2;
    constexpr int SIZE = (CHUNK_SIZE / SCALE) + 1;
    constexpr int SIZE2 = SIZE*SIZE;
    constexpr int SIZE3 = SIZE*SIZE*SIZE;

    float hmap[SIZE2];
    fn->GenUniformGrid2D(hmap, chunkWorldPosition.x / SCALE, chunkWorldPosition.z / SCALE, SIZE, SIZE, 0.01f * SCALE, 0);

    // float hmap[16*16];
    // generateHeights(hmap, chunkWorldPosition);

    float caveDensity[SIZE3];
    fn_celullarDist->GenUniformGrid3D(caveDensity, chunkWorldPosition.x / SCALE, chunkWorldPosition.y / SCALE, chunkWorldPosition.z / SCALE, SIZE, SIZE, SIZE, 0.0085f * SCALE, 0);

    // float maskmap[SIZE3];
    // fn->GenUniformGrid3D(maskmap, chunkWorldPosition.x / SCALE, chunkWorldPosition.y / SCALE, chunkWorldPosition.z / SCALE, SIZE, SIZE, SIZE, 0.0025f * SCALE, 2434);

    for (int z = 0 ; z < CHUNK_SIZE ; ++z) {
    for (int x = 0 ; x < CHUNK_SIZE ; ++x) {
        float height = getGridAtScaled2<SIZE, SCALE>(hmap, x, z);
        for (int y = 0 ; y < CHUNK_SIZE ; ++y) {

            int index = INDEX_XYZ(x, y, z);
            float world_y = chunkWorldPosition.y + y;

            if (world_y > height) continue;

            // float mask = getGridAtScaled3<SIZE, SCALE>(maskmap, x, y, z);

            float minFactor = 0.84f;// + mask * 0.1f;
            float maxFactor = 1.0f;// + mask * 0.1f;
            float heightFactor = glm::max(minFactor, mapRange(world_y, -64, height, minFactor, maxFactor));
            float threshold = heightFactor;// glm::max(heightFactor, maskmap[index] * 5.0f); // use max to not cover the map with caves

            float density = getGridAtScaled3<SIZE, SCALE>(caveDensity, x, y, z);

            if (density > threshold) {
            // if (caveDensity[index] > threshold) {
                blocks[index] = (uint8_t)BlockType::Air;
            }
        }
    }}
}


// static int findTopBlock(uint8_t* blocks, int localX, int localZ) {
//     if (blocks[INDEX_XYZ(localX, CHUNK_SIZE-1, localZ)] != (uint8_t)BlockType::Air) {
//         return -1;
//     }
//     for (int y = CHUNK_SIZE-1 ; y >= 0 ; --y) {
//         if (blocks[INDEX_XYZ(localX, y, localZ)] > 0) return y;
//     }
//     return -1;
// }
