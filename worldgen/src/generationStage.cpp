#include "generationStage.hpp"
#include "terrain/terrain.hpp"
#include "structures/vegetation.hpp"
#include "common.hpp"
#include <string.h>

static bool TT(uint8_t* blocks, const glm::ivec3& chunkWorldPos, GenerationStage stage) {
    const auto key = glm::ivec4(chunkWorldPos, stage);
    const auto it = _cache.find(key);
    if (it != _cache.end()) {
        memcpy(blocks, it->second, CHUNK_BLOCK_COUNT);
        return true;
    }
    return false;
}

static void TT2(const uint8_t* blocks, const glm::ivec3& chunkWorldPos, GenerationStage stage) {
    const auto key = glm::ivec4(chunkWorldPos, stage);

    uint8_t* p = (uint8_t*)malloc(CHUNK_BLOCK_COUNT * sizeof(uint8_t));
    memcpy(p, blocks, CHUNK_BLOCK_COUNT);
    _cache[key] = p;
}

#include <stdio.h>

// TODO: put GenerationStage as template argument
void generateStages(uint8_t* blocks, const glm::ivec3& chunkWorldPos, GenerationStage stage) {
    // const auto key = glm::ivec4(chunkWorldPos, stage);
    // const auto it = _cache.find(key);
    // if (it != _cache.end()) {
    //     memcpy(blocks, it->second, CHUNK_BLOCK_COUNT);
    //     return;
    // }

    // printf("%d\n", (int)_cache.size());
    constexpr size_t MAX_CACHE_SIZE = 20000;
    if (_cache.size() > MAX_CACHE_SIZE) {
        for (const auto& it : _cache) {
            free(it.second);
        }
        _cache.clear();
    }

    if (stage >= GenerationStage::Terrain) {
        if (!TT(blocks, chunkWorldPos, GenerationStage::Terrain)) {
            generateStage1(blocks, chunkWorldPos);
            TT2(blocks, chunkWorldPos, GenerationStage::Terrain);
        }
    }
    if (stage >= GenerationStage::Surface) {
        if (!TT(blocks, chunkWorldPos, GenerationStage::Surface)) {
            generateSurface(blocks, chunkWorldPos);
            TT2(blocks, chunkWorldPos, GenerationStage::Surface);
        }
    }
    if (stage >= GenerationStage::Caves) {
        if (!TT(blocks, chunkWorldPos, GenerationStage::Caves)) {
            generateCaves(blocks, chunkWorldPos);
            TT2(blocks, chunkWorldPos, GenerationStage::Caves);
        }
    }
    if (stage >= GenerationStage::Structures) {
        if (!TT(blocks, chunkWorldPos, GenerationStage::Structures)) {
            generateTrees(blocks, chunkWorldPos);
            TT2(blocks, chunkWorldPos, GenerationStage::Structures);
        };
    }

    // uint8_t* p = (uint8_t*)malloc(CHUNK_BLOCK_COUNT * sizeof(uint8_t));
    // memcpy(p, blocks, CHUNK_BLOCK_COUNT);
    // _cache[key] = p;
}
