#pragma once

#include <stdint.h>

#ifdef MYLIB_EXPORTS
#define MYLIB_API __declspec(dllexport)
#else
#define MYLIB_API __declspec(dllimport)
#endif


#ifdef __cplusplus
extern "C" {
#endif

MYLIB_API void genChunk(uint8_t* blocks, int32_t x, int32_t y, int32_t z);

#ifdef __cplusplus
}
#endif
