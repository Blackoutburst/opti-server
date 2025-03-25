#include "common.hpp"

float mapRange(float value, float min1, float max1, float min2, float max2) {
    return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

bool setBlock(uint8_t* blocks, uint8_t value, int localX, int localY, int localZ) {
    if (!IS_INSIDE_CHUNK(localX, localY, localZ)) return 0;
    int index = INDEX_XYZ(localX, localY, localZ);
    blocks[index] = value;
    return 1;
}
