#include <stdlib.h>
#include <string.h>
#include "network/client.h"
#include "network/packet.h"
#include "network/encoder.h"

void clientSendChunk(TCP_CLIENT* client, CHUNK* chunk) {
    C04SEND_CHUNK* packet = malloc(sizeof(C04SEND_CHUNK));
    packet->id = CLIENT_PACKET_SEND_CHUNK;
    packet->x = chunk->position.x;
    packet->y = chunk->position.y;
    packet->z = chunk->position.z;
    memcpy(packet->blocks, chunk->blocks, CHUNK_BLOCK_COUNT);

    U8* buffer = encodePacketSendChunk(packet);
    serverWrite(client, buffer, getClientPacketSize(CLIENT_PACKET_SEND_CHUNK));

    free(packet);
    free(buffer);
}

void clientSendMonotypeChunk(TCP_CLIENT* client, CHUNK* chunk) {
    C05SEND_MONOTYPE_CHUNK* packet = malloc(sizeof(C05SEND_MONOTYPE_CHUNK));
    packet->id = CLIENT_PACKET_SEND_MONOTYPE_CHUNK;
    packet->x = chunk->position.x;
    packet->y = chunk->position.y;
    packet->z = chunk->position.z;
    packet->type = chunk->blocks[0];

    U8* buffer = encodePacketSendMonotypeChunk(packet);
    serverWrite(client, buffer, getClientPacketSize(CLIENT_PACKET_SEND_MONOTYPE_CHUNK));

    free(packet);
    free(buffer);
}
