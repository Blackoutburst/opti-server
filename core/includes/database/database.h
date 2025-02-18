#pragma once

#include "utils/types.h"
#include "network/server.h"
#include "world/chunk.h"
#include "cc/cc.h"
#include "utils/vector.h"

void dbGetChunksInRegion(TCP_CLIENT* client, const vec(VECTORI)* chunksToGet);
U8* dbGetChunkBlocks(I32 x, I32 y, I32 z);
void dbAddChunks(CHUNK** chunks, U32 count);
void dbAddChunk(CHUNK* chunk);
void _dbCreateWorldTable(void);
void _dbCreateChunkTable(void);
void dbCreateTables(void);
void dbClean(void);
void dbInit(void);
