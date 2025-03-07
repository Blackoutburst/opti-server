#pragma once

float getGridAtScaled1(const float* map, int x, int scale);
float getGridAtScaled2(const float* map, int x, int y, int size, int scale);
float getGridAtScaled3(const float* map, int x, int y, int z, int size, int scale);

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
