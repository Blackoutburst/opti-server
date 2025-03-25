#pragma once

#include <unordered_map>
#include <shared_mutex>
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
inline std::shared_mutex _cache_mutex;

using generationFunction = std::function<void(uint8_t*, const glm::ivec3&)>;

template <GenerationStage stage>
inline void _generateStage(uint8_t* blocks, const glm::ivec3& chunkWorldPos, generationFunction func)
{
    const auto key = glm::ivec4(chunkWorldPos, stage);
    {
        const std::shared_lock<std::shared_mutex> lock(_cache_mutex);
        const auto it = _cache.find(key);
        if (it != _cache.end()) {
            memcpy(blocks, it->second, CHUNK_BLOCK_COUNT);
            return;
        }
    }

    func(blocks, chunkWorldPos);

    // NOTE: only cache stage Terrain, caves or surface (because they are the only being reused during generation)
    if (!(stage == GenerationStage::Terrain || stage == GenerationStage::Caves || stage == GenerationStage::Surface)) return;

    uint8_t* p = (uint8_t*)malloc(CHUNK_BLOCK_COUNT * sizeof(uint8_t));
    memcpy(p, blocks, CHUNK_BLOCK_COUNT);

    _cache_mutex.lock();
    auto [_, inserted] = _cache.try_emplace(key, p); // Would leak if only try to insert without checking if key already exist
    _cache_mutex.unlock();
    if (!inserted) {
        free(p);
    }
}

template <GenerationStage stage>
void generateStages(uint8_t* blocks, const glm::ivec3& chunkWorldPos) {
    constexpr size_t MAX_CACHE_SIZE = 10'000; // 10'000 ~= 40Mo
    if (_cache.size() > MAX_CACHE_SIZE) {
        _cache_mutex.lock();
        for (const auto& it : _cache) {
            free(it.second);
        }
        _cache.clear();
        _cache_mutex.unlock();
    }

    if (stage >= GenerationStage::Terrain)
        _generateStage<GenerationStage::Terrain>(blocks, chunkWorldPos, generateTerrain);
    if (stage >= GenerationStage::Surface)
        _generateStage<GenerationStage::Surface>(blocks, chunkWorldPos, generateSurface);
    if (stage >= GenerationStage::Caves)
        _generateStage<GenerationStage::Caves>(blocks, chunkWorldPos, generateCaves);
    if (stage >= GenerationStage::Structures)
        _generateStage<GenerationStage::Structures>(blocks, chunkWorldPos, generateTrees);
}
