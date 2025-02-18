#pragma once

#include "utils/types.h"
#include "utils/vector.h"

#define CUBE(x) (x * x * x)

#define MIN(a, b) (a > b ? b : a)
#define MAX(a, b) (a < b ? b : a)
#define MIX(x, y, a) (x*(1.0-a)+y*a)



U32 xyzToIndex(I32 x, I32 y, I32 z);
void indexToXYZ(VECTORI* vector, I16 index);
