#include <cstring>
// #include <print>

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtc/random.hpp>

#include "terrain/terrain.hpp"
#include "utils/grid.hpp"
#include "common.hpp"
#include "utils/easings.hpp"

void generateStage1(uint8_t* blocks, const glm::ivec3& chunkWorldPosition) {
    // float* v_continental = noise_continental.genGrid2D(chunkWorldPosition);
    // float* v_terrain_density = noise_terrain_density.genGrid3D(chunkWorldPosition);

    NoiseData v_continental = noise_continental.genGrid2D(chunkWorldPosition);
    NoiseData v_terrain_density = noise_terrain_density.genGrid3D(chunkWorldPosition);

    for (int dz = 0 ; dz < CHUNK_SIZE ; ++dz) {
    for (int dx = 0 ; dx < CHUNK_SIZE ; ++dx) {
        // float height = Easings::easeInOutQuint((v_continental[INDEX_XY(dx, dz)]));
        float height = Easings::easeInOutQuint(v_continental.get(dx, dz));

        float heightW = height * 200.0f + 16;

        for (int dy = 0 ; dy < CHUNK_SIZE ; ++dy) {
            int i = INDEX_XYZ(dx, dy, dz);
            float worldY = chunkWorldPosition.y + dy;
            glm::ivec3 blockWorldPosition = chunkWorldPosition + glm::ivec3(dx, dy, dz);

            // float terrainDensity = v_terrain_density[i];
            float terrainDensity = v_terrain_density.get(dx, dy, dz);
            float density = heightW + terrainDensity * 70.0f;// mapRange(terrainDensity, 0, 64, 0.0f, 0.5f);

            if (density > worldY) {
                blocks[i] = (uint8_t)BlockType::Stone;
            } else {
                if (worldY < 0) {
                    blocks[i] = (uint8_t)BlockType::Water;
                } else {
                    blocks[i] = (uint8_t)BlockType::Air;
                }
            }

        }
    }}
}

void generateSurface(uint8_t* blocks, const glm::ivec3& chunkWorldPosition) {
    for (int dz = 0 ; dz < CHUNK_SIZE ; ++dz) {
    for (int dx = 0 ; dx < CHUNK_SIZE ; ++dx) {
    for (int dy = 0 ; dy < CHUNK_SIZE-2 ; ++dy) {
        int i0 = INDEX_XYZ(dx, dy, dz);
        int i1 = INDEX_XYZ(dx, dy+1, dz);

        if (blocks[i0] == (uint8_t)BlockType::Stone && blocks[i1] == (uint8_t)BlockType::Air) {
            blocks[i0] = (uint8_t)BlockType::Grass;
        }
    }}}
}


void generateCaves(uint8_t* blocks, const glm::ivec3& chunkWorldPosition) {
    constexpr int SCALE = 2;
    constexpr int SIZE = (CHUNK_SIZE / SCALE) + 1;
    constexpr int SIZE2 = SIZE*SIZE;
    constexpr int SIZE3 = SIZE*SIZE*SIZE;

    NoiseData v_continental = noise_continental.genGrid2D(chunkWorldPosition);
    NoiseData v_caveDensity = noise_cave_density.genGrid3D(chunkWorldPosition, 0.0085f);

    for (int z = 0 ; z < CHUNK_SIZE ; ++z) {
    for (int x = 0 ; x < CHUNK_SIZE ; ++x) {
        // float height = getGridAtScaled2<SIZE, SCALE>(hmap, x, z);
        // float height = v_continental[INDEX_XY(x, z)] * 200.0f + 32;
        float height = v_continental.get(x, z) * 200.0f + 32;
        for (int y = 0 ; y < CHUNK_SIZE ; ++y) {

            int index = INDEX_XYZ(x, y, z);
            float world_y = chunkWorldPosition.y + y;

            if (world_y > height) continue;

            float minFactor = 0.84f;// + mask * 0.1f;
            float maxFactor = 1.0f;// + mask * 0.1f;
            float heightFactor = glm::max(minFactor, mapRange(world_y, -64, height, minFactor, maxFactor));
            float threshold = heightFactor;// glm::max(heightFactor, maskmap[index] * 5.0f); // use max to not cover the map with caves

            // float density = getGridAtScaled3<SIZE, SCALE>(caveDensity, x, y, z);
            // float density = v_caveDensity[INDEX_XYZ(x, y, z)];
            float density = v_caveDensity.get(x, y, z);

            if (density > threshold) {
                if (blocks[index] != (uint8_t)BlockType::Water) {
                    blocks[index] = (uint8_t)BlockType::Air;
                }
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
