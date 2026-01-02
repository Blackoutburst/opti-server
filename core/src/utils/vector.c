#include "utils/vector.h"
#include <math.h>

float vec3i_distance2(VECTORI a, VECTORI b)
{
    int dx = a.x - b.x;
    int dy = a.y - b.y;
    int dz = a.z - b.z;
    return dx*dx + dy*dy + dz*dz;
}

float vec3i_distance(VECTORI a, VECTORI b)
{
    return sqrtf(vec3i_distance2(a, b));
}
