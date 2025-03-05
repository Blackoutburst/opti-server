#pragma once

#include <cstdint>
#include <glm/glm.hpp>
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
inline FastNoise::SmartNode fn_celullarValue;
inline FastNoise::SmartNode fn_celullarDist;

inline thread_local uint8_t temp_chunk[CHUNK_BLOCK_COUNT];
// -- //
