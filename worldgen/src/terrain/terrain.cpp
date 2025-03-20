#include <cstring>
// #include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtc/random.hpp>

#include "terrain/terrain.hpp"
#include "utils/grid.hpp"
#include "common.hpp"
#include "utils/easings.hpp"

#include "generationStage.hpp"

void generateStage1(uint8_t* blocks, const glm::ivec3& chunkWorldPosition) {
    NoiseData v_continental = noise_continental.genGrid2D(chunkWorldPosition);
    NoiseData v_terrain_density = noise_terrain_density.genGrid3D(chunkWorldPosition);

    for (int dz = 0 ; dz < CHUNK_SIZE ; ++dz) {
    for (int dx = 0 ; dx < CHUNK_SIZE ; ++dx) {
        float height = Easings::easeInOutQuint(v_continental.get(dx, dz));
        float heightW = height * 200.0f + 16;

        for (int dy = 0 ; dy < CHUNK_SIZE ; ++dy) {
            int i = INDEX_XYZ(dx, dy, dz);
            float worldY = chunkWorldPosition.y + dy;
            glm::ivec3 blockWorldPosition = chunkWorldPosition + glm::ivec3(dx, dy, dz);

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
    uint8_t top_chunk[CHUNK_BLOCK_COUNT];
    generateStages(top_chunk, chunkWorldPosition + glm::ivec3(0, CHUNK_SIZE, 0), GenerationStage::Terrain);

    // top layer
    for (int dz = 0 ; dz < CHUNK_SIZE ; ++dz) {
    for (int dx = 0 ; dx < CHUNK_SIZE ; ++dx) {
        int i0 = INDEX_XYZ(dx, CHUNK_SIZE-1, dz);
        int i1 = INDEX_XYZ(dx, 0, dz);

        if (blocks[i0] == (uint8_t)BlockType::Stone && top_chunk[i1] == (uint8_t)BlockType::Air) {
            blocks[i0] = (uint8_t)BlockType::Grass;
        }
    }}

    for (int dz = 0 ; dz < CHUNK_SIZE ; ++dz) {
    for (int dx = 0 ; dx < CHUNK_SIZE ; ++dx) {
    for (int dy = 0 ; dy < CHUNK_SIZE-1 ; ++dy) {
        int i0 = INDEX_XYZ(dx, dy, dz);
        int i1 = INDEX_XYZ(dx, dy+1, dz);

        if (blocks[i0] == (uint8_t)BlockType::Stone && blocks[i1] == (uint8_t)BlockType::Air) {
            blocks[i0] = (uint8_t)BlockType::Grass;
        }
    }}}
}


// exponential
// float smin( float a, float b, float k )
// {
//     k *= 1.0;
//     float r = glm::exp2(-a/k) + glm::exp2(-b/k); // Profiling says that exp2 is slow
//     return -k*glm::log2(r);
// }

// quadratic polynomial (faster)
float smin( float a, float b, float k )
{
    k *= 4.0;
    float h = glm::max( k-glm::abs(a-b), 0.0f )/k;
    return glm::min(a,b) - h*h*k*(1.0f/4.0f);
}

void generateCaves(uint8_t* blocks, const glm::ivec3& chunkWorldPosition) {
    constexpr int SCALE = 2;
    constexpr int SIZE = (CHUNK_SIZE / SCALE) + 1;
    constexpr int SIZE2 = SIZE*SIZE;
    constexpr int SIZE3 = SIZE*SIZE*SIZE;

    NoiseData v_continental = noise_continental.genGrid2D(chunkWorldPosition);
    NoiseData v_caveDensity = noise_cave_density.genGrid3D(chunkWorldPosition, 0.0085f);
    NoiseData v_caveBig = noise_cave_density1.genGrid3D(chunkWorldPosition, 0.006f);

    for (int z = 0 ; z < CHUNK_SIZE ; ++z) {
    for (int x = 0 ; x < CHUNK_SIZE ; ++x) {
        float height = v_continental.get(x, z) * 200.0f + 32;
        for (int y = 0 ; y < CHUNK_SIZE ; ++y) {

            int index = INDEX_XYZ(x, y, z);
            float world_y = chunkWorldPosition.y + y;

            if (world_y > height) continue;

            float minFactor = 0.9f;// + mask * 0.1f;
            float maxFactor = 1.0f;// + mask * 0.1f;
            float threshold = glm::max(minFactor, mapRange(world_y, -64, height, minFactor, maxFactor));

            float density = v_caveDensity.get(x, y, z);
            float densityC = v_caveBig.get(x, y, z) * 0.5f + 0.5f;

            density = 1.0f - smin(1.0f - density, 1.0f - (float)(densityC < 0.3f), 0.05f); // smooth max du pauvre

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
