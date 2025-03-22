#pragma once

#include <unordered_map>
#include <string.h>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include "terrain/terrain.hpp"
#include "structures/vegetation.hpp"
#include "chunk.hpp"

enum class GenerationStage: uint8_t {
    Terrain,
    Surface,
    Caves,
    Structures,
    All
};

inline std::unordered_map<glm::ivec4, uint8_t*> _cache;

using generationFunction = std::function<void(uint8_t*, const glm::ivec3&)>;

template <GenerationStage stage>
inline void _generateStage(uint8_t* blocks, const glm::ivec3& chunkWorldPos, generationFunction func)
{
    const auto key = glm::ivec4(chunkWorldPos, stage);
    {
        // lock shared
        const auto it = _cache.find(key);
        if (it != _cache.end()) {
            memcpy(blocks, it->second, CHUNK_BLOCK_COUNT);
            return;
        }
    }

    func(blocks, chunkWorldPos);

    uint8_t* p = (uint8_t*)malloc(CHUNK_BLOCK_COUNT * sizeof(uint8_t));
    memcpy(p, blocks, CHUNK_BLOCK_COUNT);

    // lock
    _cache[key] = p;
    // unlock
}

template <GenerationStage stage>
void generateStages(uint8_t* blocks, const glm::ivec3& chunkWorldPos) {
    constexpr size_t MAX_CACHE_SIZE = 20'000;
    if (_cache.size() > MAX_CACHE_SIZE) {
        for (const auto& it : _cache) {
            free(it.second);
        }
        _cache.clear();
    }

    if (stage >= GenerationStage::Terrain)
        _generateStage<GenerationStage::Terrain>(blocks, chunkWorldPos, generateStage1);
    if (stage >= GenerationStage::Surface)
        _generateStage<GenerationStage::Surface>(blocks, chunkWorldPos, generateSurface);
    if (stage >= GenerationStage::Caves)
        _generateStage<GenerationStage::Caves>(blocks, chunkWorldPos, generateCaves);
    if (stage >= GenerationStage::Structures)
        _generateStage<GenerationStage::Structures>(blocks, chunkWorldPos, generateTrees);
}
