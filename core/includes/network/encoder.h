#pragma once

#include "utils/types.h"
#include "network/packet.h"

U8* encodePacketIdentification(C00IDENTIFICATION* packet);
U8* encodePacketAddEntity(C01ADD_ENTITY* packet);
U8* encodePacketRemoveEntity(C02REMOVE_ENTITY* packet);
U8* encodePacketUpdateEntity(C03UPDATE_ENTITY* packet);
U8* encodePacketSendChunk(C04SEND_CHUNK* packet);
U8* encodePacketSendMonotypeChunk(C05SEND_MONOTYPE_CHUNK* packet);
U8* encodePacketChat(C06CHAT* packet);
U8* encodePacketEntityMetadata(C07UPDATE_ENTITY_METADATA* packet);

