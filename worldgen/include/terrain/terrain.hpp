#pragma once

#include <cstdint>
#include <glm/ext/vector_int3.hpp>

void generateTerrain(uint8_t* blocks, const glm::ivec3& chunkWorldPosition);
void generateCaves(uint8_t* blocks, const glm::ivec3& chunkWorldPosition);
void generateSurface(uint8_t* blocks, const glm::ivec3& chunkWorldPosition);
