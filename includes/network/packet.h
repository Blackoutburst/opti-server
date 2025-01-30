#pragma once

#include "utils/types.h"

typedef struct C00PacketIdentification C00IDENTIFICATION;
typedef struct C01PacketAddEntity C01ADD_ENTITY;
typedef struct C02RemoveEntity C02REMOVE_ENTITY;
typedef struct C03UpdateEntity C03UPDATE_ENTITY;
typedef struct C04SendChunk C04SEND_CHUNK;
typedef struct C05SendMonotypeChunk C05SEND_MONOTYPE_CHUNK;
typedef struct C06Chat C06CHAT;
typedef struct C07UpdateEntityMetadata C07UPDATE_ENTITY_METADATA;

typedef struct BlockBulkEdit BLOCK_BULK_EDIT;

typedef struct S00UpdateEntity S00UPDATE_ENTITY;
typedef struct S01UpdateBlock S01UPDATE_BLOCK;
typedef struct S02BlockBulkEdit S02BLOCK_BULK_EDIT;
typedef struct S03Chat S03CHAT;
typedef struct S04ClientMetadata S04CLIENT_METADATA;

#pragma pack(push, 1)
struct C00PacketIdentification {
    U8 id;
    U32 entityId;
};

struct C01PacketAddEntity {
    U8 id;
    U32 entityId;
    F32 x;
    F32 y;
    F32 z;
    F32 yaw;
    F32 pitch;
    U8 name[64];
};

struct C02RemoveEntity {
    U8 id;
    U32 entityId;
};

struct C03UpdateEntity {
    U8 id;
    U32 entityId;
    F32 x;
    F32 y;
    F32 z;
    F32 yaw;
    F32 pitch;
};

struct C04SendChunk {
    U8 id;
    I32 x;
    I32 y;
    I32 z;
    U8 blocks[4096];
};

struct C05SendMonotypeChunk {
    U8 id;
    I32 x;
    I32 y;
    I32 z;
    U8 type;
};

struct C06Chat {
    U8 id;
    U8 message[4096];
};

struct C07UpdateEntityMetadata {
    U8 id;
    U32 entityId;
    U8 name[64];
};

struct S00UpdateEntity {
    F32 x;
    F32 y;
    F32 z;
    F32 yaw;
    F32 pitch;
};

struct S01UpdateBlock {
    U8 type;
    I32 x;
    I32 y;
    I32 z;
};

struct BlockBulkEdit {
    U8 type;
    I32 x;
    I32 y;
    I32 z;
};

struct S02BlockBulkEdit {
    U32 blockCount;
    BLOCK_BULK_EDIT* blocks;
};

struct S03Chat {
    U8 message[4096];
};

struct S04ClientMetadata {
    U8 id;
    U8 renderDistance;
    U8 name[64];
};
#pragma pack(pop)


enum ClientPackets {
    CLIENT_PACKET_IDENTIFICATION,
    CLIENT_PACKET_ADD_ENTITY,
    CLIENT_PACKET_REMOVE_ENTITY,
    CLIENT_PACKET_UPDATE_ENTITY,
    CLIENT_PACKET_SEND_CHUNK,
    CLIENT_PACKET_SEND_MONOTYPE_CHUNK,
    CLIENT_PACKET_CHAT,
    CLIENT_PACKET_UPDATE_ENTITY_METADATA,
};

enum ServerPackets {
    SERVER_PACKET_UPDATE_ENTITY,
    SERVER_PACKET_UPDATE_BLOCK,
    SERVER_PACKET_BLOCK_BULK_EDIT,
    SERVER_PACKET_CHAT,
    SERVER_PACKET_CLIENT_METADATA,
};

U16 getClientPacketSize(I8 packetID);
U16 getServerPacketSize(I8 packetID);
