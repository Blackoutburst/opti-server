#pragma once

#include "utils/types.h"
#include "network/packet.h"

void encodePacketIdentification(U8* buffer, C00IDENTIFICATION* packet);
void encodePacketAddEntity(U8* buffer, C01ADD_ENTITY* packet);
void encodePacketRemoveEntity(U8* buffer, C02REMOVE_ENTITY* packet);
void encodePacketUpdateEntity(U8* buffer, C03UPDATE_ENTITY* packet);
void encodePacketSendChunk(U8* buffer, C04SEND_CHUNK* packet);
void encodePacketSendMonotypeChunk(U8* buffer, C05SEND_MONOTYPE_CHUNK* packet);
void encodePacketChat(U8* buffer, C06CHAT* packet);
void encodePacketEntityMetadata(U8* buffer, C07UPDATE_ENTITY_METADATA* packet);
