#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "main.h"
#include "utils/buffer.h"
#include "utils/string.h"
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
    free(buffer);

    TCP_CLIENT** clients = getAllClients();
    for (U32 i = 0; i < MAX_TCP_CLIENT; i++) {
        if (clients[i] == NULL) continue;
        if (clients[i]->id == client->id) continue;
        clientSendUpdateEntity(clients[i], client);
    }
    
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

void clientReceiveBlockBulkEdit(TCP_CLIENT* client, U8* buffer) {
    // TODO: flem
    free(buffer);
}

void clientReceiveChat(TCP_CLIENT* client, U8* buffer) {
    serverBroadcast(buffer, sizeof(CLIENT_PACKET_CHAT));
    free(buffer);
}

void clientReceiveClientMetadata(TCP_CLIENT* client, U8* buffer) {
    S04CLIENT_METADATA* packet = decodePacketClientMetadata(buffer);

    U8 mRd = getServerMaxRenderDistance();
    U8 newRenderDistance = packet->renderDistance > mRd ? mRd : packet->renderDistance;
    
    client->renderDistance = newRenderDistance;
    memcpy(client->name, packet->name, 64);
    
    serverBroadcast(buffer, sizeof(CLIENT_PACKET_UPDATE_ENTITY_METADATA));
    free(buffer);
    free(packet);
}

void clientSendIdentification(TCP_CLIENT* client) {
    C00IDENTIFICATION packet;
    packet.id = CLIENT_PACKET_IDENTIFICATION;
    packet.entityId = getClientId();

    U8* buffer = encodePacketIdentification(&packet);
    serverWrite(client, buffer, getClientPacketSize(CLIENT_PACKET_IDENTIFICATION)); 

    free(buffer);
}

void clientSendAddEntity(TCP_CLIENT* client, TCP_CLIENT* entity) {
    C01ADD_ENTITY packet;
    packet.id = CLIENT_PACKET_ADD_ENTITY;
    packet.entityId = entity->id;
    packet.x = entity->position.x;
    packet.y = entity->position.y;
    packet.z = entity->position.z;
    packet.yaw = entity->yaw;
    packet.pitch = entity->pitch;
    memcpy(packet.name, entity->name, 64);

    U8* buffer = encodePacketAddEntity(&packet);
    serverWrite(client, buffer, getClientPacketSize(CLIENT_PACKET_ADD_ENTITY)); 

    free(buffer);
}

void clientSendRemoveEntity(TCP_CLIENT* client, U32 entityId) {
    C02REMOVE_ENTITY packet;
    packet.id = CLIENT_PACKET_REMOVE_ENTITY;
    packet.entityId = entityId;

    U8* buffer = encodePacketRemoveEntity(&packet);
    serverWrite(client, buffer, getClientPacketSize(CLIENT_PACKET_REMOVE_ENTITY)); 

    free(buffer);
}

void clientSendUpdateEntity(TCP_CLIENT* client, TCP_CLIENT* entity) {
    C03UPDATE_ENTITY packet;
    packet.id = CLIENT_PACKET_UPDATE_ENTITY;
    packet.entityId = entity->id;
    packet.x = entity->position.x;
    packet.y = entity->position.y;
    packet.z = entity->position.z;
    packet.yaw = entity->yaw;
    packet.pitch = entity->pitch;

    U8* buffer = encodePacketUpdateEntity(&packet);
    serverWrite(client, buffer, getClientPacketSize(CLIENT_PACKET_UPDATE_ENTITY)); 

    free(buffer);
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

void clientSendChat(TCP_CLIENT* client, const U8* message) {
    C06CHAT packet;
    packet.id = CLIENT_PACKET_CHAT;

    U8* encodedString = encodeString(message, 4096);
    memcpy(packet.message, encodedString, 4096);

    U8* buffer = encodePacketChat(&packet);
    serverWrite(client, buffer, getClientPacketSize(CLIENT_PACKET_CHAT));

    free(encodedString);
    free(buffer);
}

void clientSendClientMetadata(TCP_CLIENT* client, TCP_CLIENT* entity) {
    C07UPDATE_ENTITY_METADATA packet;
    packet.id = CLIENT_PACKET_UPDATE_ENTITY_METADATA;
    packet.entityId = entity->id;

    U8* encodedString = encodeString(entity->name, 64);
    memcpy(packet.name, encodedString, 64);

    U8* buffer = encodePacketEntityMetadata(&packet);
    serverWrite(client, buffer, getClientPacketSize(CLIENT_PACKET_UPDATE_ENTITY_METADATA));

    free(encodedString);
    free(buffer);
}

