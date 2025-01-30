#pragma once

#include "utils/types.h"
#include "network/packet.h"

S00UPDATE_ENTITY* decodePacketUpdateEntity(U8* buffer);
S01UPDATE_BLOCK* decodePacketUpdateBlock(U8* buffer);
S02BLOCK_BULK_EDIT* decodePacketBlockBulkEdit(U8* buffer);
S03CHAT* decodePacketChat(U8* buffer);
S04CLIENT_METADATA* decodePacketClientMetadata(U8* buffer);
