#pragma once

#include <stdint.h>

#if defined(_WIN32) || defined(_WIN64)

    #ifdef MYLIB_EXPORTS
    #define MYLIB_API __declspec(dllexport)
    #else
    #define MYLIB_API __declspec(dllimport)
    #endif

#else

#define MYLIB_API

#endif


#ifdef __cplusplus
extern "C" {
#endif

MYLIB_API void genChunk(uint8_t* blocks, int32_t x, int32_t y, int32_t z);

#ifdef __cplusplus
}
#endif
