#include <stdlib.h>
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
    // TODO
    return NULL;
}

S03CHAT* decodePacketChat(U8* buffer) {
    // TODO
    return NULL;
}

S04CLIENT_METADATA* decodePacketClientMetadata(U8* buffer) {
    // TODO
    return NULL;
}
