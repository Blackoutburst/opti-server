#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtc/random.hpp>
#include <unordered_map>
#include <tuple>

enum class GenerationStage: uint8_t {
    Terrain,
    Surface,
    Caves,
    Structures,
    All
};

inline std::unordered_map<glm::ivec4, uint8_t*> _cache;

void generateStages(uint8_t* blocks, const glm::ivec3& chunkWorldPos, GenerationStage stage);
