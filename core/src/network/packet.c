#include "network/packet.h"
#include "network/client.h"

void (*getServerPacketFunction(I8 packetId))(TCP_CLIENT*, U8*) {
    switch (packetId) {
        case SERVER_PACKET_UPDATE_ENTITY:
            return &clientReceiveUpdateEntity;
        case SERVER_PACKET_UPDATE_BLOCK:
            return &clientReceiveUpdateBlock;
        case SERVER_PACKET_BLOCK_BULK_EDIT:
            return &clientReceiveBlockBulkEdit;
        case SERVER_PACKET_CHAT:
            return &clientReceiveChat;
        case SERVER_PACKET_CLIENT_METADATA:
            return &clientReceiveClientMetadata;
        default: return NULL;
    }
}

U16 getClientPacketSize(I8 packetID) {
    switch (packetID) {
        case CLIENT_PACKET_IDENTIFICATION:
            return sizeof(C00IDENTIFICATION);
        case CLIENT_PACKET_ADD_ENTITY:
            return sizeof(C01ADD_ENTITY);
        case CLIENT_PACKET_REMOVE_ENTITY:
            return sizeof(C02REMOVE_ENTITY);
        case CLIENT_PACKET_UPDATE_ENTITY:
            return sizeof(C03UPDATE_ENTITY);
        case CLIENT_PACKET_SEND_CHUNK:
            return sizeof(C04SEND_CHUNK);
        case CLIENT_PACKET_SEND_MONOTYPE_CHUNK:
            return sizeof(C05SEND_MONOTYPE_CHUNK);
        case CLIENT_PACKET_CHAT:
            return sizeof(C06CHAT);
        case CLIENT_PACKET_UPDATE_ENTITY_METADATA:
            return sizeof(C07UPDATE_ENTITY_METADATA);
        default:
            return 0;
    }
}

U16 getServerPacketSize(I8 packetID) {
    switch (packetID) {
        case SERVER_PACKET_UPDATE_ENTITY:
            return sizeof(S00UPDATE_ENTITY);
        case SERVER_PACKET_UPDATE_BLOCK:
            return sizeof(S01UPDATE_BLOCK);
        case SERVER_PACKET_BLOCK_BULK_EDIT:
            return 4; // Block count as U32 the rest depend on this
        case SERVER_PACKET_CHAT:
            return sizeof(S03CHAT);
        case SERVER_PACKET_CLIENT_METADATA:
            return sizeof(S04CLIENT_METADATA);
        default:
            return 0;
    }
}
