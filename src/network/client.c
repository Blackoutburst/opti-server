#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "utils/buffer.h"
#include "utils/math.h"
#include "database/database.h"
#include "network/client.h"
#include "network/packet.h"
#include "network/encoder.h"
#include "network/decoder.h"
#include "world/world.h"
#include "world/chunk.h"

void clientReceiveUpdateEntity(TCP_CLIENT* client, U8* buffer) {
    S00UPDATE_ENTITY* packet = decodePacketUpdateEntity(buffer);
    
    client->position.x = packet->x;
    client->position.y = packet->y;
    client->position.z = packet->z;
    client->yaw = packet->yaw;
    client->pitch = packet->pitch;
    
    free(packet);
    
    worldUpdateClientChunk(client);
}

void clientReceiveUpdateBlock(TCP_CLIENT* client, U8* buffer) {
    S01UPDATE_BLOCK* packet = decodePacketUpdateBlock(buffer);
    U8 type = packet->type;
    I32 x = packet->x;
    I32 y = packet->y;
    I32 z = packet->z; 

    free(packet);
    
    CHUNK* chunk = NULL;
    I32 cx = TO_CHUNK_POS(x);
    I32 cy = TO_CHUNK_POS(y);
    I32 cz = TO_CHUNK_POS(z);
    U8* data = dbGetChunkBlocks(cx, cy, cz);
    if (data == NULL) {
        chunk = chunkCreate(cx, cy, cz);
    } else {
        chunk = chunkAssemble(cx, cy, cz, data);
    }
    
    if (chunk->monotype) {
        U8 t = chunk->blocks[0];
        free(chunk->blocks);
        chunk->blocks = malloc(CHUNK_BLOCK_COUNT);
        for (I32 i = 0; i < CHUNK_BLOCK_COUNT; i++) chunk->blocks[i] = t;
        chunk->monotype = 0;
    }

    U32 index = xyzToIndex(x - cx, y - cy, z - cz);
    chunk->blocks[index] = type;

    chunk->monotype = chunkIsMonotype(chunk);

    if (chunk->monotype) {
        C05SEND_MONOTYPE_CHUNK p;
        p.id = CLIENT_PACKET_SEND_MONOTYPE_CHUNK;
        p.x = chunk->position.x;
        p.y = chunk->position.y;
        p.z = chunk->position.z;
        p.type = chunk->blocks[0];

        U8* buffer = encodePacketSendMonotypeChunk(&p);
        serverBroadcast(buffer, getClientPacketSize(CLIENT_PACKET_SEND_MONOTYPE_CHUNK));
        free(buffer);
    } else {
        C04SEND_CHUNK p;
        p.id = CLIENT_PACKET_SEND_CHUNK;
        p.x = chunk->position.x;
        p.y = chunk->position.y;
        p.z = chunk->position.z;
        for (U32 i = 0; i < CHUNK_BLOCK_COUNT; i++) p.blocks[i] = chunk->blocks[i];

        U8* buffer = encodePacketSendChunk(&p);
        serverBroadcast(buffer, getClientPacketSize(CLIENT_PACKET_SEND_CHUNK));
        free(buffer);
    }
    
    dbAddChunk(chunk);
    chunkClean(chunk);
}

void clientSendChunk(TCP_CLIENT* client, CHUNK* chunk) {
    C04SEND_CHUNK packet;
    packet.id = CLIENT_PACKET_SEND_CHUNK;
    packet.x = chunk->position.x;
    packet.y = chunk->position.y;
    packet.z = chunk->position.z;
    for (U32 i = 0; i < CHUNK_BLOCK_COUNT; i++) packet.blocks[i] = chunk->blocks[i];

    U8* buffer = encodePacketSendChunk(&packet);
    serverWrite(client, buffer, getClientPacketSize(CLIENT_PACKET_SEND_CHUNK));

    free(buffer);
}

void clientSendMonotypeChunk(TCP_CLIENT* client, CHUNK* chunk) {
    C05SEND_MONOTYPE_CHUNK packet;
    packet.id = CLIENT_PACKET_SEND_MONOTYPE_CHUNK;
    packet.x = chunk->position.x;
    packet.y = chunk->position.y;
    packet.z = chunk->position.z;
    packet.type = chunk->blocks[0];

    U8* buffer = encodePacketSendMonotypeChunk(&packet);
    serverWrite(client, buffer, getClientPacketSize(CLIENT_PACKET_SEND_MONOTYPE_CHUNK));

    free(buffer);
}
