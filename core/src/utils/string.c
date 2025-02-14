#include "utils/string.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

U8* encodeString(const U8* str, U16 size) {
    U8* string = malloc(sizeof(U8) * size);
    for (U16 i = 0; i < size; i++) string[i] = 0;

    for (U16 i = 0; i < size && str[i]; i++) string[i] = str[i];

    return string;
}

I8* copyString(const I8* str) {
    if (str == NULL) return NULL;

    const I32 length = strlen(str);
    I8* s = malloc(length + 1);
    memcpy(s, str, length);

    return s;
}
