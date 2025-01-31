#pragma once

#include "utils/types.h"
#include "utils/vector.h"
#include "world/chunk.h"

// Forwarding from server.h //
typedef struct tcpClient TCP_CLIENT;
struct tcpClient;
//////////////////////////////

typedef struct chunkHashmap CHUNK_HASHMAP;

struct chunkHashmap {
    VECTORI position;
    CHUNK* chunk;
    U8 used;
};

CHUNK* worldLoadChunk(TCP_CLIENT* client);
void worldUnloadChunk(TCP_CLIENT* client);
void worldRemoveChunkOutOfRenderDistance(TCP_CLIENT* client);
void worldUpdateClientChunk(TCP_CLIENT* client);
