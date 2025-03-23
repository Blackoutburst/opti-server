#pragma once

#define CHUNK_SIZE (16)
#define CHUNK_BLOCK_COUNT (CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE)
#define INDEX_XYZ(x, y, z) ((z) * CHUNK_SIZE*CHUNK_SIZE + (y) * CHUNK_SIZE + (x))
#define INDEX_XY(x, y) ((y) * CHUNK_SIZE + (x))
#define IS_INSIDE_CHUNK(x, y, z) (((x) >= 0 && (x) < CHUNK_SIZE && (y) >= 0 && (y) < CHUNK_SIZE && (z) >= 0 && (z) < CHUNK_SIZE))
