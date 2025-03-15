#pragma once

#include <cstdint>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <FastNoise/FastNoise.h>

#define CHUNK_SIZE (16)
#define CHUNK_BLOCK_COUNT (CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE)
#define INDEX_XYZ(x, y, z) ((z) * CHUNK_SIZE*CHUNK_SIZE + (y) * CHUNK_SIZE + (x))
#define INDEX_XY(x, y) ((y) * CHUNK_SIZE + (x))
#define IS_INSIDE_CHUNK(x, y, z) (((x) >= 0 && (x) < CHUNK_SIZE && (y) >= 0 && (y) < CHUNK_SIZE && (z) >= 0 && (z) < CHUNK_SIZE))

enum class BlockType : uint8_t {
    Air = 0,
    Grass = 1,
    Dirt = 2,
    Stone = 3,
    OakLog = 4,
    OakLeaves = 5,
    Glass = 6,
    Water = 7,
    Sand = 8,
    Snow = 9,
    OakPlank = 10,
    StoneBrick = 11,
    Netherrack = 12,
    Gold = 13,
    PackedIce = 14,
    Lava = 15,
    Barrel = 16,
    Bookshelf = 17,

    INVALID,
};

float mapRange(float value, float min1, float max1, float min2, float max2);
void setBlock(uint8_t* blocks, uint8_t value, int localX, int localY, int localZ);
void generateHeights(float map[CHUNK_SIZE*CHUNK_SIZE], const glm::ivec3& chunkWorldPosition);


// Global variables //
inline FastNoise::SmartNode fn;
inline FastNoise::SmartNode fn_terrain;
inline FastNoise::SmartNode fn_celullarValue;
inline FastNoise::SmartNode fn_celullarDist;

inline FastNoise::SmartNode noise_continental; // 2D - height
inline FastNoise::SmartNode noise_terrain_density; // 3D -
// inline FastNoise::SmartNode noise_cave_density; // 3D -


using NoiseCache = std::unordered_map<glm::ivec3, float*>;

inline NoiseCache cache_noise_terrain_density; // 3D
inline NoiseCache cache_noise_continental; // 2D


inline float* getNoiseCache(NoiseCache& cache, const glm::ivec3& pos, int dimensions, const FastNoise::SmartNode<>& noise) {
    const auto it = cache.find(pos);
    if (it != cache.end()) {
        return it->second;
    }

    constexpr int MAX_SIZE = 1000;
    if (cache.size() > MAX_SIZE) {
        for (const auto& it : cache) {
            free(it.second);
        }
        cache.clear();
    }

    float* v;
    if (dimensions == 2) {
        v = (float*)malloc(CHUNK_SIZE*CHUNK_SIZE * sizeof(float));
        noise->GenUniformGrid2D(v, pos.x, pos.z, CHUNK_SIZE, CHUNK_SIZE, 1.0f, 0);
    }
    else if (dimensions == 3) {
        v = (float*)malloc(CHUNK_BLOCK_COUNT * sizeof(float));
        noise->GenUniformGrid3D(v, pos.x, pos.y, pos.z, CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE, 1.0f, 0);
    }

    cache[pos] = v;
    return v;
}

inline thread_local uint8_t temp_chunk[CHUNK_BLOCK_COUNT];
// -- //
