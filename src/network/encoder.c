#include <stdlib.h>
#include "network/encoder.h"
#include "utils/buffer.h"

U8* encodePacketIdentification(C00IDENTIFICATION* packet) {
    U8* buffer = malloc(sizeof(C00IDENTIFICATION));
    for (U16 i = 0; i < sizeof(C00IDENTIFICATION); i++) buffer[i] = 0;
    U32 index = 0;

    putU8(&index, packet->id, buffer);
    putU32(&index, packet->entityId, buffer);

    return buffer;
}

U8* encodePacketAddEntity(C01ADD_ENTITY* packet) {
    U8* buffer = malloc(sizeof(C01ADD_ENTITY));
    for (U16 i = 0; i < sizeof(C01ADD_ENTITY); i++) buffer[i] = 0;
    U32 index = 0;

    putU8(&index, packet->id, buffer);
    putU32(&index, packet->entityId, buffer);
    putF32(&index, packet->x, buffer);
    putF32(&index, packet->y, buffer);
    putF32(&index, packet->z, buffer);
    putF32(&index, packet->yaw, buffer);
    putF32(&index, packet->pitch, buffer);
    for (U16 i = 0; i < 64; i++) putU8(&index, packet->name[i], buffer);

    return buffer;
}

U8* encodePacketRemoveEntity(C02REMOVE_ENTITY* packet) {
    U8* buffer = malloc(sizeof(C02REMOVE_ENTITY));
    for (U16 i = 0; i < sizeof(C02REMOVE_ENTITY); i++) buffer[i] = 0;
    U32 index = 0;

    putU8(&index, packet->id, buffer);
    putU32(&index, packet->entityId, buffer);

    return buffer;
}

U8* encodePacketUpdateEntity(C03UPDATE_ENTITY* packet) {
    U8* buffer = malloc(sizeof(C03UPDATE_ENTITY));
    for (U16 i = 0; i < sizeof(C03UPDATE_ENTITY); i++) buffer[i] = 0;
    U32 index = 0;

    putU8(&index, packet->id, buffer);
    putU32(&index, packet->entityId, buffer);
    putF32(&index, packet->x, buffer);
    putF32(&index, packet->y, buffer);
    putF32(&index, packet->z, buffer);
    putF32(&index, packet->yaw, buffer);
    putF32(&index, packet->pitch, buffer);


    return buffer;
}

U8* encodePacketSendChunk(C04SEND_CHUNK* packet) {
    U8* buffer = malloc(sizeof(C04SEND_CHUNK));
    for (U16 i = 0; i < sizeof(C04SEND_CHUNK); i++) buffer[i] = 0;
    U32 index = 0;

    putU8(&index, packet->id, buffer);
    putI32(&index, packet->x, buffer);
    putI32(&index, packet->y, buffer);
    putI32(&index, packet->z, buffer);
    for (U16 i = 0; i < 4096; i++) putU8(&index, packet->blocks[i], buffer);

    return buffer;
}

U8* encodePacketSendMonotypeChunk(C05SEND_MONOTYPE_CHUNK* packet) {
    U8* buffer = malloc(sizeof(C05SEND_MONOTYPE_CHUNK));
    for (U16 i = 0; i < sizeof(C05SEND_MONOTYPE_CHUNK); i++) buffer[i] = 0;
    U32 index = 0;

    putU8(&index, packet->id, buffer);
    putI32(&index, packet->x, buffer);
    putI32(&index, packet->y, buffer);
    putI32(&index, packet->z, buffer);
    putU8(&index, packet->type, buffer);

    return buffer;
}

U8* encodePacketChat(C06CHAT* packet) {
    U8* buffer = malloc(sizeof(C06CHAT));
    for (U16 i = 0; i < sizeof(C06CHAT); i++) buffer[i] = 0;
    U32 index = 0;

    putU8(&index, packet->id, buffer);
    for (U16 i = 0; i < 4096; i++) putU8(&index, packet->message[i], buffer);

    return buffer;
}

U8* encodePacketEntityMetadata(C07UPDATE_ENTITY_METADATA* packet) {
    U8* buffer = malloc(sizeof(C07UPDATE_ENTITY_METADATA));
    for (U16 i = 0; i < sizeof(C07UPDATE_ENTITY_METADATA); i++) buffer[i] = 0;
    U32 index = 0;

    putU8(&index, packet->id, buffer);
    putU32(&index, packet->entityId, buffer);
    for (U16 i = 0; i < 64; i++) putU8(&index, packet->name[i], buffer);

    return buffer;
}

