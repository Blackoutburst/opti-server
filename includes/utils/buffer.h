#pragma once

#include "utils/types.h"

void printBufferHex(const I8 *title, const U8 *buf, U32 buf_len);
I8 getI8(U8** buffer);
U8 getU8(U8** buffer);
I16 getI16(U8** buffer);
U16 getU16(U8** buffer);
I32 getI32(U8** buffer);
U32 getU32(U8** buffer);
I64 getI64(U8** buffer);
U64 getU64(U8** buffer);
F32 getF32(U8** buffer);
F64 getF64(U8** buffer);
void putI8(U16* index, I8 value, U8* buffer);
void putU8(U16* index, U8 value, U8* buffer);
void putI16(U16* index, I16 value, U8* buffer);
void putU16(U16* index, U16 value, U8* buffer);
void putI32(U16* index, I32 value, U8* buffer);
void putU32(U16* index, U32 value, U8* buffer);
void putI64(U16* index, I64 value, U8* buffer);
void putU64(U16* index, U64 value, U8* buffer);
void putF32(U16* index, F32 value, U8* buffer);
void putF64(U16* index, F64 value, U8* buffer);
