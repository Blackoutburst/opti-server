#pragma once

#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <FastNoise/FastNoise.h>

#include "chunk.hpp"

template<int SIZE, int SCALE>
class NoiseData {
public:
    NoiseData(float* data): _data(data) {}

    float get(int x, int y) {
        return getGridAtScaled2<SIZE, SCALE>(_data, x, y);
    }

    float get(int x, int y, int z) {
        return getGridAtScaled3<SIZE, SCALE>(_data, x, y, z);
    }

private:
    float* _data = nullptr;
};


using NoiseCache = std::unordered_map<glm::ivec3, float*>;

constexpr int SCALE = 8;
constexpr int SIZE = (CHUNK_SIZE / SCALE) + 1;

class Noise {
    const int MAX_CACHE_SIZE = 1000;

public:
    Noise() = default;
    ~Noise() {
        cleanCache(_cache2D);
        cleanCache(_cache3D);
    }

    void assign(const FastNoise::SmartNode<FastNoise::Generator>& noise) {
        _noise = noise;
    }

    void setSeed(int seed) {
        _seed = seed;
    }

    float genSingle2D(int x, int y) {
        return _noise->GenSingle2D(x, y, _seed);
    }

    float genSingle3D(int x, int y, int z) {
        return _noise->GenSingle3D(x, y, z, _seed);
    }

    NoiseData<SIZE, SCALE> genGrid2D(const glm::ivec3& pos, float frequency = 1.0f) {
    // float* genGrid2D(const glm::ivec3& pos, float frequency = 1.0f) {
        const auto it = _cache2D.find(pos);
        if (it != _cache2D.end()) {
            // return it->second;
            return NoiseData<SIZE, SCALE>(it->second);
        }

        if (_cache2D.size() > MAX_CACHE_SIZE) {
            cleanCache(_cache2D);
        }

        float* v = (float*)malloc(SIZE*SIZE * sizeof(float));
        _noise->GenUniformGrid2D(v, pos.x / SCALE, pos.z / SCALE, SIZE, SIZE, frequency * SCALE, _seed);

        _cache2D[pos] = v;
        return NoiseData<SIZE, SCALE>(v);
    }

    NoiseData<SIZE, SCALE> genGrid3D(const glm::ivec3& pos, float frequency = 1.0f) {
        const auto it = _cache3D.find(pos);
        if (it != _cache3D.end()) {
            // return it->second;
            return NoiseData<SIZE, SCALE>(it->second);
        }

        if (_cache3D.size() > MAX_CACHE_SIZE) {
            cleanCache(_cache3D);
        }

        float* v = (float*)malloc(SIZE*SIZE*SIZE * sizeof(float));
        _noise->GenUniformGrid3D(v, pos.x/SCALE, pos.y/SCALE, pos.z/SCALE, SIZE, SIZE, SIZE, frequency * SCALE, _seed);

        _cache3D[pos] = v;
        // return v;
        return NoiseData<SIZE, SCALE>(v);
    }

private:
    void cleanCache(NoiseCache& cache) {
        for (const auto& it : cache) {
            free(it.second);
        }
        cache.clear();
    }

private:
    FastNoise::SmartNode<FastNoise::Generator> _noise;
    NoiseCache _cache2D;
    NoiseCache _cache3D;
    int _seed = 0;
};
