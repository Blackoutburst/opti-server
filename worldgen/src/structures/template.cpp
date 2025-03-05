// static void placeTemplate(uint8_t* blocks, uint8_t* template_blocks, glm::ivec3 localPos, int sizeX, int sizeY, int sizeZ) {
//     for (int32_t dz = 0 ; dz < sizeZ ; ++dz) {
//     for (int32_t dy = 0 ; dy < sizeY ; ++dy) {
//     for (int32_t dx = 0 ; dx < sizeX ; ++dx) {
//         int32_t value_index = (dz*sizeZ*sizeZ) + ((sizeY-1-dy)*sizeY) + dx;
//         uint8_t value = template_blocks[value_index];

//         if (value == 0) continue;

//         int32_t x = localPos.x + dx;
//         int32_t y = localPos.y + dy;
//         int32_t z = localPos.z + dz;

//         if (!IS_INSIDE_CHUNK(x, y, z)) continue;

//         blocks[INDEX_XYZ(x, y, z)] = value;
//     }}}
// }
