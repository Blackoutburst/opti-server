#include <stdlib.h>
#include <string.h>
#include "utils/buffer.h"
#include "network/decoder.h"


S00UPDATE_ENTITY* decodePacketUpdateEntity(U8* buffer) {
    U8** bufferPtr = &buffer;

    S00UPDATE_ENTITY* packet = malloc(sizeof(S00UPDATE_ENTITY));
    packet->x = getF32(bufferPtr);
    packet->y = getF32(bufferPtr);
    packet->z = getF32(bufferPtr);
    packet->yaw = getF32(bufferPtr);
    packet->pitch = getF32(bufferPtr);

    return packet;
}

S01UPDATE_BLOCK* decodePacketUpdateBlock(U8* buffer) {
    U8** bufferPtr = &buffer;

    S01UPDATE_BLOCK* packet = malloc(sizeof(S01UPDATE_BLOCK));
    packet->type = getU8(bufferPtr);
    packet->x = getI32(bufferPtr);
    packet->y = getI32(bufferPtr);
    packet->z = getI32(bufferPtr);

    return packet;
}

S02BLOCK_BULK_EDIT* decodePacketBlockBulkEdit(U8* buffer) {
    U8** bufferPtr = &buffer;
    U32 blockCount = getU32(bufferPtr);

    S02BLOCK_BULK_EDIT* packet = malloc(sizeof(S02BLOCK_BULK_EDIT));
    packet->blockCount = blockCount;
    printf("%i %i %i\n", sizeof(BLOCK_BULK_EDIT), blockCount, sizeof(BLOCK_BULK_EDIT) * blockCount);
    packet->blocks = malloc(sizeof(BLOCK_BULK_EDIT) * blockCount);

    for (U32 i = 0; i < blockCount; i++) {
        packet->blocks[i].type = getU8(bufferPtr);
        packet->blocks[i].x = getI32(bufferPtr);
        packet->blocks[i].y = getI32(bufferPtr);
        packet->blocks[i].z = getI32(bufferPtr);
    }

    return packet;
}

S03CHAT* decodePacketChat(U8* buffer) {
    U8** bufferPtr = &buffer;

    S03CHAT* packet = malloc(sizeof(S03CHAT));
    U8 msg[4096];
    for (I32 i = 0; i < 4096; i++) msg[i] = getU8(bufferPtr);

    memcpy(packet->message, msg, 4096);

    return packet;
}

S04CLIENT_METADATA* decodePacketClientMetadata(U8* buffer) {
    U8** bufferPtr = &buffer;

    S04CLIENT_METADATA* packet = malloc(sizeof(S04CLIENT_METADATA));
    packet->renderDistance = getU8(bufferPtr);
    U8 n[64];
    for (I32 i = 0; i < 64; i++) n[i] = getU8(bufferPtr);

    memcpy(packet->name, n, 64);

    return packet;
}
