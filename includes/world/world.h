#pragma once

#include "utils/types.h"
#include "utils/vector.h"
#include "world/chunk.h"
#include "network/server.h"
#include "cc/cc.h"

U8 worldGetChunk(TCP_CLIENT* client, I32 x, I32 y, I32 z);
void worldAddChunk(TCP_CLIENT* client, CHUNK* chunk);
CHUNK* worldLoadChunk(TCP_CLIENT* client, I32 x, I32 y, I32 z);
void worldUnloadChunk(TCP_CLIENT* client, I32 x, I32 y, I32 z);
void worldRemoveChunkOutOfRenderDistance(TCP_CLIENT* client);
void worldUpdateClientChunk(TCP_CLIENT* client);
