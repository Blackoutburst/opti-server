#pragma once

#include "utils/types.h"
#include "world/chunk.h"

U8* dbGetChunkBlocks(I32 x, I32 y, I32 z);
void dbAddChunk(CHUNK* chunk);
void _dbCreateWorldTable(void);
void _dbCreateChunkTable(void);
void dbCreateTables(void);
void dbClean(void);
void dbInit(void);
