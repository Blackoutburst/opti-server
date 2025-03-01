#include "utils/RLEencoding.h"

I32 rleCompression(U8* dest, const U8* src, I32 size) {
    I32 offset = 0;
    RLE_VALUE_TYPE prevValue = src[0];
    RLE_COUNT_TYPE count = 0;

    for (int i = 0 ; i < size ; ++i) {
        if (src[i] != prevValue) {
            *((RLE_COUNT_TYPE*)&(dest[offset*RLE_SIZE]))                  = count;
            *((RLE_VALUE_TYPE*)&(dest[offset*RLE_SIZE + RLE_COUNT_SIZE])) = prevValue;
            offset += 1;
            count = 1;
        } else {
            count += 1;
        }

        prevValue = src[i];
    }

    *((RLE_COUNT_TYPE*)&(dest[offset*RLE_SIZE]))  = count; // To write I32 into U8 array
    dest[offset*RLE_SIZE + RLE_COUNT_SIZE] = prevValue;
    offset += 1;

    return offset * RLE_SIZE;
}

I32 rleDecompression(U8* dest, const U8* src, I32 size) {
    const I32 N = size / RLE_SIZE;
    I32 offset = 0;

    // TODO: offset need to be adapted in case something else than U8 is used as VALUE_TYPE

    for (int i = 0 ; i < N ; ++i) {
        RLE_COUNT_TYPE count = *((RLE_COUNT_TYPE*)&(src[i * RLE_SIZE]));
        RLE_VALUE_TYPE value = *((RLE_VALUE_TYPE*)&(src[i * RLE_SIZE + RLE_COUNT_SIZE]));

        for (int j = 0 ; j < count ; ++j) {
            dest[offset++] = value;
        }
    }

    return offset;
}
