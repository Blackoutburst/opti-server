#pragma once

#include "utils/types.h"
#include "utils/vector.h"
#include "world/chunk.h"
#include "network/server.h"

CHUNK* worldGetChunk(TCP_CLIENT* client, I32 x, I32 y, I32 z);
CHUNK* worldLoadChunk(TCP_CLIENT* client, I32 x, I32 y, I32 z);
void worldUnloadChunk(TCP_CLIENT* client, I32 x, I32 y, I32 z);
void worldRemoveChunkOutOfRenderDistance(TCP_CLIENT* client);
void worldUpdateClientChunk(TCP_CLIENT* client);
