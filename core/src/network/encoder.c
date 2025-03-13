#include <stdlib.h>
#include "network/encoder.h"
#include "utils/buffer.h"
#include "world/chunk.h"

void encodePacketIdentification(U8* buffer, C00IDENTIFICATION* packet) {
    U32 index = 0;

    putU8(&index, packet->id, buffer);
    putU32(&index, packet->entityId, buffer);
}

void encodePacketAddEntity(U8* buffer, C01ADD_ENTITY* packet) {
    U32 index = 0;

    putU8(&index, packet->id, buffer);
    putU32(&index, packet->entityId, buffer);
    putF32(&index, packet->x, buffer);
    putF32(&index, packet->y, buffer);
    putF32(&index, packet->z, buffer);
    putF32(&index, packet->yaw, buffer);
    putF32(&index, packet->pitch, buffer);
    for (U16 i = 0; i < 64; i++) putU8(&index, packet->name[i], buffer);
}

void encodePacketRemoveEntity(U8* buffer, C02REMOVE_ENTITY* packet) {
    U32 index = 0;

    putU8(&index, packet->id, buffer);
    putU32(&index, packet->entityId, buffer);
}

void encodePacketUpdateEntity(U8* buffer, C03UPDATE_ENTITY* packet) {
    U32 index = 0;

    putU8(&index, packet->id, buffer);
    putU32(&index, packet->entityId, buffer);
    putF32(&index, packet->x, buffer);
    putF32(&index, packet->y, buffer);
    putF32(&index, packet->z, buffer);
    putF32(&index, packet->yaw, buffer);
    putF32(&index, packet->pitch, buffer);
}

void encodePacketSendChunk(U8* buffer, C04SEND_CHUNK* packet) {
    U32 index = 0;

    putU8(&index, packet->id, buffer);
    putI32(&index, packet->x, buffer);
    putI32(&index, packet->y, buffer);
    putI32(&index, packet->z, buffer);
    for (U16 i = 0; i < CHUNK_BLOCK_COUNT; i++) putU8(&index, packet->blocks[i], buffer);
}

void encodePacketSendMonotypeChunk(U8* buffer, C05SEND_MONOTYPE_CHUNK* packet) {
    U32 index = 0;

    putU8(&index, packet->id, buffer);
    putI32(&index, packet->x, buffer);
    putI32(&index, packet->y, buffer);
    putI32(&index, packet->z, buffer);
    putU8(&index, packet->type, buffer);
}

void encodePacketChat(U8* buffer, C06CHAT* packet) {
    U32 index = 0;

    putU8(&index, packet->id, buffer);
    for (U16 i = 0; i < 4096; i++) putU8(&index, packet->message[i], buffer);
}

void encodePacketEntityMetadata(U8* buffer, C07UPDATE_ENTITY_METADATA* packet) {
    U32 index = 0;

    putU8(&index, packet->id, buffer);
    putU32(&index, packet->entityId, buffer);
    for (U16 i = 0; i < 64; i++) putU8(&index, packet->name[i], buffer);
}
