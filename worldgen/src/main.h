// #ifdef __cplusplus
// #define EXTERNC extern "C"
// #else
// #define EXTERNC
// #endif

// typedef void* mylibrary_mytype_t;

// EXTERNC mylibrary_mytype_t mylibrary_mytype_init();
// EXTERNC void mylibrary_mytype_destroy(mylibrary_mytype_t mytype);
// EXTERNC void mylibrary_mytype_doit(mylibrary_mytype_t self, int param);

// #undef EXTERNC
// ...

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern "C" void genChunk(uint8_t* blocks, int32_t x, int32_t y, int32_t z);

#ifdef __cplusplus
}
#endif


// #ifdef __cplusplus
// extern "C" {
// #endif

// // __declspec( dllexport )
// // __declspec( dllimport )


// #ifdef __cplusplus
// }
// #endif
