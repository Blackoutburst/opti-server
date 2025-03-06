#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include "glm/gtx/hash.hpp"
#include "glm/gtc/random.hpp"

#include "terrain/terrain.hpp"
#include "utils/easings.hpp"
#include "utils/grid.hpp"
#include "common.hpp"


void generateStage1(uint8_t* blocks, const glm::ivec3& chunkWorldPosition) {
    float hmap[16*16];
    generateHeights(hmap, chunkWorldPosition);

    float cmap[16*16];
    fn_celullarValue->GenUniformGrid2D(cmap, chunkWorldPosition.x, chunkWorldPosition.z, 16, 16, 0.0025f, 0);

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

        float height = hmap[iXZ];// (2 + hmap[iXZ]) * 15.0f;

        height += cmap[INDEX_XY(dx, dz)] * 25.0f;

        if (blockWorldPosition.y >= height) {
            blocks[i] = (uint8_t)BlockType::Air;
        } else if (blockWorldPosition.y >= height - 1.0f) {
            blocks[i] = (uint8_t)BlockType::Grass;
        } else if (blockWorldPosition.y >= height - 5.0f) {
            blocks[i] = (uint8_t)BlockType::Dirt;
        } else {
            blocks[i] = (uint8_t)BlockType::Stone;
        }
    }}}
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


// static int findTopBlock(uint8_t* blocks, int localX, int localZ) {
//     if (blocks[INDEX_XYZ(localX, CHUNK_SIZE-1, localZ)] != (uint8_t)BlockType::Air) {
//         return -1;
//     }
//     for (int y = CHUNK_SIZE-1 ; y >= 0 ; --y) {
//         if (blocks[INDEX_XYZ(localX, y, localZ)] > 0) return y;
//     }
//     return -1;
// }

void generateStage2(uint8_t* blocks, const glm::ivec3& chunkWorldPosition) {
    float hmap[16*16];
    generateHeights(hmap, chunkWorldPosition);

    float caveDensity[CHUNK_BLOCK_COUNT];
    fn_celullarDist->GenUniformGrid3D(caveDensity, chunkWorldPosition.x, chunkWorldPosition.y, chunkWorldPosition.z, 16, 16, 16, 0.0085f, 0);

    float maskmap[CHUNK_BLOCK_COUNT];
    fn->GenUniformGrid3D(maskmap, chunkWorldPosition.x, chunkWorldPosition.y, chunkWorldPosition.z, 16, 16, 16, 0.0025f, 2434);


    for (int z = 0 ; z < CHUNK_SIZE ; ++z) {
    for (int y = 0 ; y < CHUNK_SIZE ; ++y) {
    for (int x = 0 ; x < CHUNK_SIZE ; ++x) {
        int index = INDEX_XYZ(x, y, z);

        float world_y = chunkWorldPosition.y + y;

        float height = hmap[INDEX_XY(x, z)];

        float mask = maskmap[index];
        float minFactor = 0.84f;// + mask * 0.1f;
        float maxFactor = 1.0f;// + mask * 0.1f;
        float heightFactor = glm::max(minFactor, mapRange(world_y, -64, height, minFactor, maxFactor));
        float threshold = heightFactor;// glm::max(heightFactor, maskmap[index] * 5.0f); // use max to not cover the map with caves

        if (caveDensity[index] > threshold) {
            blocks[index] = (uint8_t)BlockType::Air;
        }
    }}}
}
