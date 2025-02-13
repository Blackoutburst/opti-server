#include <stdio.h>

#include "utils/buffer.h"

void printBufferHex(const I8 *title, const U8 *buf, U32 buf_len) {
    printf("%s [ ", title);

    for (U32 i = 0 ; i < buf_len ; ++i) {
        printf("%02X%s", buf[i], ( i + 1 ) % 16 == 0 ? "\r\n" : " " );
    }

    printf("]\n");
}

I8 getI8(U8** buffer)  {
    I8 data = (I8)**buffer;
    (*buffer)++;

    return data;
}

U8 getU8(U8** buffer) {
    U8 data = (U8)**buffer;
    (*buffer)++;

    return data;
}

I16 getI16(U8** buffer) {
    I16 data = (I16)(
        ((*buffer)[0] << 8) |
        ((*buffer)[1])
    );
    (*buffer) += 2;

    return data;
}

U16 getU16(U8** buffer) {
    U16 data = (U16)(
        ((*buffer)[0] << 8) |
        ((*buffer)[1])
    );
    (*buffer) += 2;

    return data;
}

I32 getI32(U8** buffer) {
    I32 data = (I32)(
        ((*buffer)[0] << 24) |
        ((*buffer)[1] << 16) |
        ((*buffer)[2] <<  8) |
        ((*buffer)[3]      )
    );
    (*buffer) += 4;

    return data;
}

U32 getU32(U8** buffer) {
    U32 data = (U32)(
        ((*buffer)[0] << 24) |
        ((*buffer)[1] << 16) |
        ((*buffer)[2] <<  8) |
        ((*buffer)[3]      )
    );
    (*buffer) += 4;

    return data;
}

I64 getI64(U8** buffer) {
    I64 data = (I64)(
        ((I64)((*buffer)[0]) << 56) |
        ((I64)((*buffer)[1]) << 48) |
        ((I64)((*buffer)[2]) << 40) |
        ((I64)((*buffer)[3]) << 32) |
        ((I64)((*buffer)[4]) << 24) |
        ((I64)((*buffer)[5]) << 16) |
        ((I64)((*buffer)[6]) <<  8) |
        ((I64)((*buffer)[7])      )
    );
    (*buffer) += 8;

    return data;
}

U64 getU64(U8** buffer) {
    U64 data = (U64)(
        ((U64)((*buffer)[0]) << 56) |
        ((U64)((*buffer)[1]) << 48) |
        ((U64)((*buffer)[2]) << 40) |
        ((U64)((*buffer)[3]) << 32) |
        ((U64)((*buffer)[4]) << 24) |
        ((U64)((*buffer)[5]) << 16) |
        ((U64)((*buffer)[6]) <<  8) |
        ((U64)((*buffer)[7])      )
    );
    (*buffer) += 8;

    return data;
}

F32 getF32(U8** buffer) {
    union {
        I32 i;
        F32 f;
    } u;

    u.i = getU32(buffer);

    return u.f;
}

F64 getF64(U8** buffer) {
    union {
        I64 i;
        F64 f;
    } u;

    u.i = getU64(buffer);

    return u.f;
}

void putI8(U32* index, I8 value, U8* buffer)  {
    buffer[*index] = value;

    *index += sizeof(I8);
}

void putU8(U32* index, U8 value, U8* buffer) {
    buffer[*index] = value;

   *index += sizeof(U8);
}

void putI16(U32* index, I16 value, U8* buffer) {
    buffer[*index  ] = (value >> 8) & 0xFF;
    buffer[*index+1] = (value)      & 0xFF;

    *index += sizeof(I16);
}

void putU16(U32* index, U16 value, U8* buffer) {
    buffer[*index  ] = (value >> 8) & 0xFF;
    buffer[*index+1] = (value)      & 0xFF;

    *index += sizeof(U16);
}

void putI32(U32* index, I32 value, U8* buffer) {
    buffer[*index  ] = (value >> 24) & 0xFF;
    buffer[*index+1] = (value >> 16) & 0xFF;
    buffer[*index+2] = (value >> 8 ) & 0xFF;
    buffer[*index+3] = (value)       & 0xFF;

    *index += sizeof(I32);
}

void putU32(U32* index, U32 value, U8* buffer) {
    buffer[*index  ] = (value >> 24) & 0xFF;
    buffer[*index+1] = (value >> 16) & 0xFF;
    buffer[*index+2] = (value >> 8 ) & 0xFF;
    buffer[*index+3] = (value)       & 0xFF;

    *index += sizeof(U32);
}

void putI64(U32* index, I64 value, U8* buffer) {
    buffer[*index  ] = (value >> 56) & 0xFF;
    buffer[*index+1] = (value >> 48) & 0xFF;
    buffer[*index+2] = (value >> 40) & 0xFF;
    buffer[*index+3] = (value >> 32) & 0xFF;
    buffer[*index+4] = (value >> 24) & 0xFF;
    buffer[*index+5] = (value >> 16) & 0xFF;
    buffer[*index+6] = (value >> 8 ) & 0xFF;
    buffer[*index+7] = (value)       & 0xFF;

    *index += sizeof(I64);
}

void putU64(U32* index, U64 value, U8* buffer) {
    buffer[*index  ] = (value >> 56) & 0xFF;
    buffer[*index+1] = (value >> 48) & 0xFF;
    buffer[*index+2] = (value >> 40) & 0xFF;
    buffer[*index+3] = (value >> 32) & 0xFF;
    buffer[*index+4] = (value >> 24) & 0xFF;
    buffer[*index+5] = (value >> 16) & 0xFF;
    buffer[*index+6] = (value >> 8 ) & 0xFF;
    buffer[*index+7] = (value)       & 0xFF;

    *index += sizeof(U64);
}

void putF32(U32* index, F32 value, U8* buffer) {
    union {
        F32 f;
        U32 u;
    } u;

    u.f = value;

    buffer[*index  ] = (u.u >> 24) & 0xFF;
    buffer[*index+1] = (u.u >> 16) & 0xFF;
    buffer[*index+2] = (u.u >> 8 ) & 0xFF;
    buffer[*index+3] = (u.u)       & 0xFF;

    *index += sizeof(F32);
}

void putF64(U32* index, F64 value, U8* buffer) {
    union {
        F64 f;
        U64 u;
    } u;

    u.f = value;

    buffer[*index  ] = (u.u >> 56) & 0xFF;
    buffer[*index+1] = (u.u >> 48) & 0xFF;
    buffer[*index+2] = (u.u >> 40) & 0xFF;
    buffer[*index+3] = (u.u >> 32) & 0xFF;
    buffer[*index+4] = (u.u >> 24) & 0xFF;
    buffer[*index+5] = (u.u >> 16) & 0xFF;
    buffer[*index+6] = (u.u >> 8 ) & 0xFF;
    buffer[*index+7] = (u.u)       & 0xFF;

    *index += sizeof(F64);
}
