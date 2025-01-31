#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "network/client.h"
#include "network/packet.h"
#include "network/encoder.h"
#include "world/world.h"

void clientReceiveUpdateEntity(TCP_CLIENT* client, U8* buffer) {
    worldUpdateClientChunk(client);
}

void clientReceiveUpdateBlock(TCP_CLIENT* client, U8* buffer) {
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
    printf("ian il fap\n");
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
    printf("ian il mono fap\n");
}
