#include "common.hpp"

float mapRange(float value, float min1, float max1, float min2, float max2) {
    return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

void setBlock(uint8_t* blocks, uint8_t value, int localX, int localY, int localZ) {
    if (!IS_INSIDE_CHUNK(localX, localY, localZ)) return;
    int index = INDEX_XYZ(localX, localY, localZ);
    blocks[index] = value;
}

void generateHeights(float map[CHUNK_SIZE*CHUNK_SIZE], const glm::ivec3& chunkWorldPosition) {
    fn->GenUniformGrid2D(map, chunkWorldPosition.x, chunkWorldPosition.z, 16, 16, 0.01f, 0);

    for (int z = 0 ; z < CHUNK_SIZE ; ++z) {
    for (int x = 0 ; x < CHUNK_SIZE ; ++x) {
        int index = INDEX_XY(x, z);
        map[index] = (2 + map[index]) * 15.0f;
    }}
}

// void generateDensity(float map[CHUNK_BLOCK_COUNT, const glm::ivec3& chunkWorldPosition) {
//     fn->GenUniformGrid2D(map, chunkWorldPosition.x, chunkWorldPosition.z, 16, 16, 0.01f, 0);

//     for (int z = 0 ; z < CHUNK_SIZE ; ++z) {
//     for (int x = 0 ; x < CHUNK_SIZE ; ++x) {
//         int index = INDEX_XY(x, z);
//         map[index] = (2 + map[index]) * 15.0f;
//     }}
// }
