#pragma once

#include <unordered_map>
#include <shared_mutex>
#include <memory>
#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <FastNoise/FastNoise.h>
#include "grid.hpp"
#include "chunk.hpp"

template<int SIZE, int SCALE>
class NoiseData {
public:
    NoiseData(std::shared_ptr<float[]> data): _data(data) {}
    // NoiseData(float* data): _data(std::make_unique<float*>(data)) {}
    // NoiseData(std::unique_ptr<float*> &&data): _data(data) {}

    float get(int x, int y) const {
        return getGridAtScaled2<SIZE, SCALE>(_data.get(), x, y);
    }

    float get(int x, int y, int z) const {
        return getGridAtScaled3<SIZE, SCALE>(_data.get(), x, y, z);
    }

private:
    // float* _data = nullptr;
    // std::unique_ptr<float*> _data;
    std::shared_ptr<float[]> _data;
};

using NoiseCache = std::unordered_map<glm::ivec3, float*>;

template<int SCALE>
class Noise {
    static constexpr int MAX_CACHE_SIZE = 1500;
    static constexpr int SIZE = (CHUNK_SIZE / SCALE) + 1; // TODO: if scale is equal to 1, don't need +1

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

    float genSingle2D(int x, int y) const {
        return _noise->GenSingle2D(x, y, _seed);
    }

    float genSingle3D(int x, int y, int z) const  {
        return _noise->GenSingle3D(x, y, z, _seed);
    }

    NoiseData<SIZE, SCALE> genGrid2D(const glm::ivec3& pos, float frequency = 1.0f) {
        auto data = std::make_shared<float[]>(SIZE*SIZE);

        {
            const std::shared_lock<std::shared_mutex> lock(_cache2D_mutex);
            const auto it = _cache2D.find(pos);
            if (it != _cache2D.end()) {
                std::memcpy(data.get(), it->second, SIZE*SIZE * sizeof(float));
                return NoiseData<SIZE, SCALE>(data);
            }
        }

        float* v = (float*)malloc(SIZE*SIZE * sizeof(float));
        _noise->GenUniformGrid2D(v, pos.x / SCALE, pos.z / SCALE, SIZE, SIZE, frequency * SCALE, _seed);
        std::memcpy(data.get(), v, SIZE*SIZE * sizeof(float));

        {
            const std::lock_guard<std::shared_mutex> lock(_cache2D_mutex);
            if (_cache2D.size() > MAX_CACHE_SIZE) {
                cleanCache(_cache2D);
            }
            _cache2D[pos] = v;
        }

        return NoiseData<SIZE, SCALE>(data);
    }

    NoiseData<SIZE, SCALE> genGrid3D(const glm::ivec3& pos, float frequency = 1.0f) {
        auto data = std::make_shared<float[]>(SIZE*SIZE*SIZE);

        {
            const std::shared_lock<std::shared_mutex> lock(_cache3D_mutex);
            const auto it = _cache3D.find(pos);
            if (it != _cache3D.end()) {
                std::memcpy(data.get(), it->second, SIZE*SIZE*SIZE * sizeof(float));
                return NoiseData<SIZE, SCALE>(data);
            }
        }

        float* v = (float*)malloc(SIZE*SIZE*SIZE * sizeof(float));
        _noise->GenUniformGrid3D(v, pos.x/SCALE, pos.y/SCALE, pos.z/SCALE, SIZE, SIZE, SIZE, frequency * SCALE, _seed);
        std::memcpy(data.get(), v, SIZE*SIZE*SIZE * sizeof(float));

        {
            const std::lock_guard<std::shared_mutex> lock(_cache3D_mutex);
            if (_cache3D.size() > MAX_CACHE_SIZE) {
                cleanCache(_cache3D);
            }
            _cache3D[pos] = v;
        }

        return NoiseData<SIZE, SCALE>(data);
    }

private:
    void cleanCache(NoiseCache& cache) { // TODO: Fix code to works multithreaded
        for (const auto& it : cache) {
            free(it.second);
        }
        cache.clear();
    }

private:
    FastNoise::SmartNode<FastNoise::Generator> _noise;
    int _seed = 0;

    NoiseCache _cache2D;
    NoiseCache _cache3D;

    std::shared_mutex _cache2D_mutex;
    std::shared_mutex _cache3D_mutex;
};
