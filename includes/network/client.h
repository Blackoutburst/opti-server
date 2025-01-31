#pragma once

#include "utils/types.h"
#include "world/chunk.h"
#include "network/server.h"

void clientReceiveUpdateEntity(TCP_CLIENT* client, U8* buffer);
void clientReceiveUpdateBlock(TCP_CLIENT* client, U8* buffer);
void clientSendChunk(TCP_CLIENT* client, CHUNK* chunk);
void clientSendMonotypeChunk(TCP_CLIENT* client, CHUNK* chunk);
