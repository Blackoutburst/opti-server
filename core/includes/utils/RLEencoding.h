#pragma once

#include "utils/types.h"

#define RLE_COUNT_TYPE U16
#define RLE_VALUE_TYPE U8

#define RLE_COUNT_SIZE (sizeof(RLE_COUNT_TYPE))
#define RLE_VALUE_SIZE (sizeof(RLE_VALUE_TYPE))
#define RLE_SIZE       (RLE_COUNT_SIZE + RLE_VALUE_SIZE)

I32 rleCompression(U8* dest, const U8* src, I32 size);
I32 rleDecompression(U8* dest, const U8* src, I32 size);
