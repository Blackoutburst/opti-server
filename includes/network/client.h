#pragma once

#include "utils/types.h"
#include "world/chunk.h"
#include "network/server.h"

void clientSendChunk(TCP_CLIENT* client, CHUNK* chunk);
void clientSendMonotypeChunk(TCP_CLIENT* client, CHUNK* chunk);
