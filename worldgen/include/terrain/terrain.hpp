#pragma once

#include <cstdint>
#include <glm/glm.hpp>

void generateStage1(uint8_t* blocks, const glm::ivec3& chunkWorldPosition);
void generateStage2(uint8_t* blocks, const glm::ivec3& chunkWorldPosition);
