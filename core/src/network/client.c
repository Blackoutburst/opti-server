#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "utils/args.h"
#include "utils/string.h"
#include "utils/math.h"
#include "utils/logger.h"
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

    I32 px = TO_CHUNK_POS((I32)client->position.x);
    I32 py = TO_CHUNK_POS((I32)client->position.y);
    I32 pz = TO_CHUNK_POS((I32)client->position.z);

    if (px != client->chunkPosition.x || py != client->chunkPosition.y || pz != client->chunkPosition.z)
        worldUpdateClientChunk(client);

    client->chunkPosition.x = px;
    client->chunkPosition.y = py;
    client->chunkPosition.z = pz;
}

void clientReceiveUpdateBlock(TCP_CLIENT* client, U8* buffer) {
    S01UPDATE_BLOCK* packet = decodePacketUpdateBlock(buffer);
    U8 type = packet->type;
    I32 x = packet->x;
    I32 y = packet->y;
    I32 z = packet->z;

    free(packet);
    free(buffer);

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

        const U32 packetSize = getClientPacketSize(CLIENT_PACKET_SEND_MONOTYPE_CHUNK);
        U8 tempBuff[packetSize];
        encodePacketSendMonotypeChunk(tempBuff, &p);

        TCP_CLIENT** tcpClients = getAllClients();
        for (U32 i = 0; i < MAX_TCP_CLIENT; i++) {
            if (tcpClients[i] == NULL) continue;
            if (p.x < tcpClients[i]->chunkPosition.x - tcpClients[i]->renderDistance * CHUNK_SIZE ||
                p.x > tcpClients[i]->chunkPosition.x + tcpClients[i]->renderDistance * CHUNK_SIZE ||
                p.y < tcpClients[i]->chunkPosition.y - tcpClients[i]->renderDistance * CHUNK_SIZE ||
                p.y > tcpClients[i]->chunkPosition.y + tcpClients[i]->renderDistance * CHUNK_SIZE ||
                p.z < tcpClients[i]->chunkPosition.z - tcpClients[i]->renderDistance * CHUNK_SIZE ||
                p.z > tcpClients[i]->chunkPosition.z + tcpClients[i]->renderDistance * CHUNK_SIZE
            ) continue;
            serverWrite(tcpClients[i], tempBuff, packetSize);
        }
    } else {
        C04SEND_CHUNK p;
        p.id = CLIENT_PACKET_SEND_CHUNK;
        p.x = chunk->position.x;
        p.y = chunk->position.y;
        p.z = chunk->position.z;
        for (U32 i = 0; i < CHUNK_BLOCK_COUNT; i++) p.blocks[i] = chunk->blocks[i];

        const U32 packetSize = getClientPacketSize(CLIENT_PACKET_SEND_CHUNK);
        U8 tempBuff[packetSize];
        encodePacketSendChunk(tempBuff, &p);

        TCP_CLIENT** tcpClients = getAllClients();
        for (U32 i = 0; i < MAX_TCP_CLIENT; i++) {
            if (tcpClients[i] == NULL) continue;
            if (p.x < tcpClients[i]->chunkPosition.x - tcpClients[i]->renderDistance * CHUNK_SIZE ||
                p.x > tcpClients[i]->chunkPosition.x + tcpClients[i]->renderDistance * CHUNK_SIZE ||
                p.y < tcpClients[i]->chunkPosition.y - tcpClients[i]->renderDistance * CHUNK_SIZE ||
                p.y > tcpClients[i]->chunkPosition.y + tcpClients[i]->renderDistance * CHUNK_SIZE ||
                p.z < tcpClients[i]->chunkPosition.z - tcpClients[i]->renderDistance * CHUNK_SIZE ||
                p.z > tcpClients[i]->chunkPosition.z + tcpClients[i]->renderDistance * CHUNK_SIZE
            ) continue;
            serverWrite(tcpClients[i], tempBuff, packetSize);
        }
    }

    dbAddChunk(chunk);
    chunkClean(chunk);
}

void clientReceiveBlockBulkEdit(TCP_CLIENT* client, U8* buffer) {
    S02BLOCK_BULK_EDIT* packet = decodePacketBlockBulkEdit(buffer);
    U32 blockCount = packet->blockCount;
    BLOCK_BULK_EDIT* blocks = packet->blocks;

    // map(U32, CHUNK*) editedChunks;
    map(VECTORI, CHUNK*) editedChunks;
    init(&editedChunks);

    for (U32 i = 0; i < blockCount; i++) {
        U8 type = blocks[i].type;
        I32 x = blocks[i].x;
        I32 y = blocks[i].y;
        I32 z = blocks[i].z;

        CHUNK* chunk = NULL;
        I32 cx = TO_CHUNK_POS(x);
        I32 cy = TO_CHUNK_POS(y);
        I32 cz = TO_CHUNK_POS(z);

        // CHUNK** oldChunk = get(&editedChunks, chunkHash(cx, cy, cz));
        CHUNK** oldChunk = get(&editedChunks, ((VECTORI){cx, cy, cz}));
        if (oldChunk != NULL) chunk = *oldChunk;

        if (chunk == NULL) {
            U8* data = dbGetChunkBlocks(cx, cy, cz);
            if (data == NULL) {
                chunk = chunkCreate(cx, cy, cz);
            } else {
                chunk = chunkAssemble(cx, cy, cz, data);
            }
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

        // if (oldChunk == NULL) insert(&editedChunks, chunkHash(cx, cy, cz), chunk);
        if (oldChunk == NULL) insert(&editedChunks, ((VECTORI){cx, cy, cz}), chunk);
    }

    TCP_CLIENT** tcpClients = getAllClients();
    for_each(&editedChunks, key, value) {
        CHUNK* chunk = *value;

        if (chunk->monotype) {
            C05SEND_MONOTYPE_CHUNK p;
            p.id = CLIENT_PACKET_SEND_MONOTYPE_CHUNK;
            p.x = chunk->position.x;
            p.y = chunk->position.y;
            p.z = chunk->position.z;
            p.type = chunk->blocks[0];

            U8 tempBuff[5000];
            encodePacketSendMonotypeChunk(tempBuff, &p);

            for (U32 i = 0; i < MAX_TCP_CLIENT; i++) {
                if (tcpClients[i] == NULL) continue;
                if (p.x < tcpClients[i]->chunkPosition.x - tcpClients[i]->renderDistance * CHUNK_SIZE ||
                    p.x > tcpClients[i]->chunkPosition.x + tcpClients[i]->renderDistance * CHUNK_SIZE ||
                    p.y < tcpClients[i]->chunkPosition.y - tcpClients[i]->renderDistance * CHUNK_SIZE ||
                    p.y > tcpClients[i]->chunkPosition.y + tcpClients[i]->renderDistance * CHUNK_SIZE ||
                    p.z < tcpClients[i]->chunkPosition.z - tcpClients[i]->renderDistance * CHUNK_SIZE ||
                    p.z > tcpClients[i]->chunkPosition.z + tcpClients[i]->renderDistance * CHUNK_SIZE
                ) continue;
                serverWrite(tcpClients[i], tempBuff, getClientPacketSize(CLIENT_PACKET_SEND_MONOTYPE_CHUNK));
            }
        } else {
            C04SEND_CHUNK p;
            p.id = CLIENT_PACKET_SEND_CHUNK;
            p.x = chunk->position.x;
            p.y = chunk->position.y;
            p.z = chunk->position.z;
            for (U32 i = 0; i < CHUNK_BLOCK_COUNT; i++) p.blocks[i] = chunk->blocks[i];

            const U32 packetSize = getClientPacketSize(CLIENT_PACKET_SEND_CHUNK);
            U8 tempBuff[packetSize];
            encodePacketSendChunk(tempBuff, &p);

            for (U32 i = 0; i < MAX_TCP_CLIENT; i++) {
                if (tcpClients[i] == NULL) continue;
                if (p.x < tcpClients[i]->chunkPosition.x - tcpClients[i]->renderDistance * CHUNK_SIZE ||
                    p.x > tcpClients[i]->chunkPosition.x + tcpClients[i]->renderDistance * CHUNK_SIZE ||
                    p.y < tcpClients[i]->chunkPosition.y - tcpClients[i]->renderDistance * CHUNK_SIZE ||
                    p.y > tcpClients[i]->chunkPosition.y + tcpClients[i]->renderDistance * CHUNK_SIZE ||
                    p.z < tcpClients[i]->chunkPosition.z - tcpClients[i]->renderDistance * CHUNK_SIZE ||
                    p.z > tcpClients[i]->chunkPosition.z + tcpClients[i]->renderDistance * CHUNK_SIZE
                ) continue;
                serverWrite(tcpClients[i], tempBuff, packetSize);
            }
        }
        dbAddChunk(chunk);
        chunkClean(chunk);
    }

    free(packet->blocks);
    free(packet);
    free(buffer);

    cleanup(&editedChunks);
}

void clientReceiveChat(TCP_CLIENT* client, U8* buffer) {
    S03CHAT* packet = decodePacketChat(buffer);
    free(buffer);

    C06CHAT newPacket;
    newPacket.id = CLIENT_PACKET_CHAT;
    U8 message[4096];
    U8* encodedName = encodeString(client->name, 64);
    U8* encodedMessage = encodeString(packet->message, 4096);

    snprintf((I8*)message, 4096, "%s: %s", encodedName, encodedMessage);
    free(encodedName);
    free(encodedMessage);

    memcpy(newPacket.message, message, 4096);
    logI("%s\n", message);

    const I32 packetSize = getClientPacketSize(CLIENT_PACKET_CHAT);
    U8 tempBuff[packetSize];
    encodePacketChat(tempBuff, &newPacket);

    serverBroadcast(tempBuff, getClientPacketSize(CLIENT_PACKET_CHAT));
    free(packet);
}

void clientReceiveClientMetadata(TCP_CLIENT* client, U8* buffer) {
    S04CLIENT_METADATA* packet = decodePacketClientMetadata(buffer);
    free(buffer);

    U8 mRd = argsGetRenderDistance();
    U8 newRenderDistance = packet->renderDistance > mRd ? mRd : packet->renderDistance;

    client->renderDistance = newRenderDistance;
    memcpy(client->name, packet->name, 64);

    C07UPDATE_ENTITY_METADATA newPacket;
    newPacket.id = CLIENT_PACKET_UPDATE_ENTITY_METADATA;
    newPacket.entityId = client->id;

    U8* encodedName = encodeString(client->name, 64);
    memcpy(newPacket.name, encodedName, 64);
    free(encodedName);

    logI("Client %i new render distance %i new name %s\n", client->id, client->renderDistance, client->name);

    const I32 packetSize = getClientPacketSize(CLIENT_PACKET_UPDATE_ENTITY_METADATA);
    U8 tempBuff[packetSize];
    encodePacketEntityMetadata(tempBuff, &newPacket);

    serverBroadcast(tempBuff, packetSize);
    free(packet);
}

void clientSendIdentification(TCP_CLIENT* client) {
    C00IDENTIFICATION packet;
    packet.id = CLIENT_PACKET_IDENTIFICATION;
    packet.entityId = getClientId();

    const I32 packetSize = getClientPacketSize(CLIENT_PACKET_IDENTIFICATION);
    U8 buffer[packetSize];
    encodePacketIdentification(buffer, &packet);
    serverWrite(client, buffer, packetSize);
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

    const I32 packetSize = getClientPacketSize(CLIENT_PACKET_ADD_ENTITY);
    U8 buffer[packetSize];
    encodePacketAddEntity(buffer, &packet);
    serverWrite(client, buffer, packetSize);
}

void clientSendRemoveEntity(TCP_CLIENT* client, U32 entityId) {
    C02REMOVE_ENTITY packet;
    packet.id = CLIENT_PACKET_REMOVE_ENTITY;
    packet.entityId = entityId;

    const I32 packetSize = getClientPacketSize(CLIENT_PACKET_REMOVE_ENTITY);
    U8 buffer[packetSize];
    encodePacketRemoveEntity(buffer, &packet);
    serverWrite(client, buffer, packetSize);
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

    const I32 packetSize = getClientPacketSize(CLIENT_PACKET_UPDATE_ENTITY);
    U8 buffer[packetSize];
    encodePacketUpdateEntity(buffer, &packet);
    serverWrite(client, buffer, packetSize);
}

void clientSendChunk(TCP_CLIENT* client, CHUNK* chunk) {
    C04SEND_CHUNK packet;
    packet.id = CLIENT_PACKET_SEND_CHUNK;
    packet.x = chunk->position.x;
    packet.y = chunk->position.y;
    packet.z = chunk->position.z;
    for (U32 i = 0; i < CHUNK_BLOCK_COUNT; i++) packet.blocks[i] = chunk->blocks[i];

    const U32 packetSize = getClientPacketSize(CLIENT_PACKET_SEND_CHUNK);
    U8 buffer[packetSize];
    encodePacketSendChunk(buffer, &packet);
    serverWrite(client, buffer, packetSize);
}

void clientSendMonotypeChunk(TCP_CLIENT* client, CHUNK* chunk) {
    C05SEND_MONOTYPE_CHUNK packet;
    packet.id = CLIENT_PACKET_SEND_MONOTYPE_CHUNK;
    packet.x = chunk->position.x;
    packet.y = chunk->position.y;
    packet.z = chunk->position.z;
    packet.type = chunk->blocks[0];

    const U32 packetSize = getClientPacketSize(CLIENT_PACKET_SEND_MONOTYPE_CHUNK);
    U8 buffer[packetSize];
    encodePacketSendMonotypeChunk(buffer, &packet);
    serverWrite(client, buffer, packetSize);
}

void clientSendChat(TCP_CLIENT* client, const U8* message) {
    C06CHAT packet;
    packet.id = CLIENT_PACKET_CHAT;

    U8* encodedString = encodeString(message, 4096);
    memcpy(packet.message, encodedString, 4096);

    const I32 packetSize = getClientPacketSize(CLIENT_PACKET_CHAT);
    U8 buffer[packetSize];
    encodePacketChat(buffer, &packet);
    serverWrite(client, buffer, packetSize);

    free(encodedString);
}

void clientSendClientMetadata(TCP_CLIENT* client, TCP_CLIENT* entity) {
    C07UPDATE_ENTITY_METADATA packet;
    packet.id = CLIENT_PACKET_UPDATE_ENTITY_METADATA;
    packet.entityId = entity->id;

    U8* encodedString = encodeString(entity->name, 64);
    memcpy(packet.name, encodedString, 64);

    const I32 packetSize = getClientPacketSize(CLIENT_PACKET_UPDATE_ENTITY_METADATA);
    U8 buffer[packetSize];
    encodePacketEntityMetadata(buffer, &packet);
    serverWrite(client, buffer, packetSize);

    free(encodedString);
}
