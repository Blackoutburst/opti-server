#include <stdlib.h>
#include "utils/string.h"

U8* encodeString(const U8* str, U16 size) {
    U8* string = malloc(sizeof(U8) * size);
    for (U16 i = 0; i < size; i++) string[i] = 0;

    for (U16 i = 0; i < size && str[i]; i++) string[i] = str[i];

    return string;
}
