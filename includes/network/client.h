#pragma once

#include "utils/types.h"
#include "world/chunk.h"
#include "network/server.h"

void clientReceiveUpdateEntity(TCP_CLIENT* client, U8* buffer);
void clientReceiveUpdateBlock(TCP_CLIENT* client, U8* buffer);
void clientReceiveBlockBulkEdit(TCP_CLIENT* client, U8* buffer);
void clientReceiveChat(TCP_CLIENT* client, U8* buffer);
void clientReceiveClientMetadata(TCP_CLIENT* client, U8* buffer);
void clientSendIdentification(TCP_CLIENT* client);
void clientSendAddEntity(TCP_CLIENT* client, TCP_CLIENT* entity);
void clientSendRemoveEntity(TCP_CLIENT* client, U32 entityId);
void clientSendUpdateEntity(TCP_CLIENT* client, TCP_CLIENT* entity);
void clientSendChunk(TCP_CLIENT* client, CHUNK* chunk);
void clientSendMonotypeChunk(TCP_CLIENT* client, CHUNK* chunk);
void clientSendChat(TCP_CLIENT* client, const U8* message);
void clientSendClientMetadata(TCP_CLIENT* client, TCP_CLIENT* entity);

