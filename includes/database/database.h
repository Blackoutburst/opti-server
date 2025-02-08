#pragma once

#include "utils/types.h"
#include "network/server.h"
#include "world/chunk.h"

void dbGetChunksInRegion(TCP_CLIENT* client, I32 minX, I32 maxX, I32 minY, I32 maxY, I32 minZ, I32 maxZ);
U8* dbGetChunkBlocks(I32 x, I32 y, I32 z);
void dbAddChunks(CHUNK* chunks, U32 count);
void dbAddChunk(CHUNK* chunk);
void _dbCreateWorldTable(void);
void _dbCreateChunkTable(void);
void dbCreateTables(void);
void dbClean(void);
void dbInit(void);
