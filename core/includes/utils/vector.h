#pragma once

#include "utils/types.h"

typedef struct vectorf VECTORF;
typedef struct vectori VECTORI;

struct vectorf {
    F32 x;
    F32 y;
    F32 z;
};

struct vectori {
    I32 x;
    I32 y;
    I32 z;
};
