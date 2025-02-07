#pragma once

#include "utils/types.h"
#include "utils/vector.h"

#define CUBE(x) (x * x * x)

U32 xyzToIndex(I32 x, I32 y, I32 z);
void indexToXYZ(VECTORI* vector, I16 index);
