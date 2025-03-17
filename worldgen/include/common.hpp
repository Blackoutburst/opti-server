#pragma once

#include <cstdint>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <FastNoise/FastNoise.h>

#include "utils/NoiseWrapper.hpp"
#include "chunk.hpp"

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

// Global variables //
inline Noise<4> noise_terrain_density;
inline Noise<8> noise_continental;
inline Noise<2> noise_cave_density;
// -- //
