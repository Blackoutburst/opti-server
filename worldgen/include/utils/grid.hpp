#pragma once

float getGridAtScaled1(const float* map, int x, int scale);
float getGridAtScaled2(const float* map, int x, int y, int size, int scale);
// float getGridAtScaled3(const float* map, int x, int y, int z, int size, int scale);

#include <glm/glm.hpp>

template<int SIZE, int SCALE>
inline float getGridAtScaled2(const float* map, int x, int y)
{
    constexpr float D = 1.0f / SCALE;
    const float fix = ((float)x) * D;
    const float fiy = ((float)y) * D;

    const int ix = static_cast<int>(fix);
    const int iy = static_cast<int>(fiy);

    const float restx = fix - ix;
    const float resty = fiy - iy;

    const float* ptr = map + iy * SIZE + ix;
    const float ma = glm::mix(ptr[0], ptr[1], restx);
    const float mb = glm::mix(ptr[SIZE], ptr[SIZE+1], restx);
    return glm::mix(ma, mb, resty);
}

template<int SIZE, int SCALE>
inline float getGridAtScaled3(const float* map, int x, int y, int z)
{
    constexpr int SIZE2 = SIZE*SIZE;
    constexpr float D = 1.0f / SCALE;

    const float fix = static_cast<float>(x) * D;
    const float fiy = static_cast<float>(y) * D;
    const float fiz = static_cast<float>(z) * D;

    const int ix = static_cast<int>(fix);
    const int iy = static_cast<int>(fiy);
    const int iz = static_cast<int>(fiz);

    const float restx = fix - ix;
    const float resty = fiy - iy;
    const float restz = fiz - iz;

    const float* __restrict ptr = map + iz*SIZE2 + iy*SIZE + ix;

    float ma = glm::mix(ptr[0], ptr[1], restx);
    float mb = glm::mix(ptr[SIZE], ptr[SIZE+1], restx);

    float mc = glm::mix(ptr[SIZE2], ptr[SIZE2+1], restx);
    float md = glm::mix(ptr[SIZE2+SIZE], ptr[SIZE2+SIZE+1], restx);

    float me = glm::mix(ma, mb, resty);
    float mf = glm::mix(mc, md, resty);

    float mg = glm::mix(me, mf, restz);

    return mg;
}
