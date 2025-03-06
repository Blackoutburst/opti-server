#include "utils/grid.hpp"
#include <glm/glm.hpp>
#include <glm/common.hpp>

// map is size one more than it's size for interpolation purposes
// void getGridAtScaled(const float* map, int x, int y, int scale)
float getGridAtScaled1(const float* map, int x, int scale)
{
    float fi = (float)x / scale;

    int ia = (int)fi;
    int ib = (int)fi + 1;

    float value_a = map[ia];
    float value_b = map[ib];

    // float rest = 1.0;//glm::modf(2.4f, 1.0f);
    float rest = glm::mod(fi, 1.0f);
    float v = glm::mix(value_a, value_b, rest);

    return v;
    // printf("fi: %.2f, ia: %d, ib: %d, value_a: %.2f, value_b: %.2f, rest: %.2f, value: %.2f\n", fi, ia, ib, value_a, value_b, rest, v);
}

/*
0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
0 2 4 6 8 10 12 14 + 16

7 / 2 = 3.5
mix(6 8, 0.5) = 7

getGridAtScaled(map, 7, 2); -> 7
*/

// size is the width|height of size of map
float getGridAtScaled2(const float* map, int x, int y, int size, int scale)
{
    float fix = (float)x / scale;
    float fiy = (float)y / scale;

    int i00 = (int)(fiy + 0)*size + (int)fix + 0;
    int i01 = (int)(fiy + 0)*size + (int)fix + 1;

    int i10 = (int)(fiy + 1)*size + (int)fix + 0;
    int i11 = (int)(fiy + 1)*size + (int)fix + 1;

    float restx = glm::mod(fix, 1.0f);
    float resty = glm::mod(fiy, 1.0f);

    float value_00 = map[i00];
    float value_10 = map[i01];

    float value_01 = map[i10];
    float value_11 = map[i11];

    float ma = glm::mix(value_00, value_10, restx);
    float mb = glm::mix(value_01, value_11, restx);
    float mc = glm::mix(ma, mb, resty);

    return mc;
}

float getGridAtScaled3(const float* map, int x, int y, int z, int size, int scale)
{
    float fix = (float)x / scale;
    float fiy = (float)y / scale;
    float fiz = (float)z / scale;

    int ix = (int)fix;
    int iy = (int)fiy;
    int iz = (int)fiz;

    const int sqSize = size*size;
    int i000 = (int)(fiz + 0)*sqSize + (int)(fiy + 0)*size + (int)fix + 0;
    int i001 = (int)(fiz + 0)*sqSize + (int)(fiy + 0)*size + (int)fix + 1;
    int i010 = (int)(fiz + 0)*sqSize + (int)(fiy + 1)*size + (int)fix + 0;
    int i011 = (int)(fiz + 0)*sqSize + (int)(fiy + 1)*size + (int)fix + 1;

    int i100 = (int)(fiz + 1)*sqSize + (int)(fiy + 0)*size + (int)fix + 0;
    int i101 = (int)(fiz + 1)*sqSize + (int)(fiy + 0)*size + (int)fix + 1;
    int i110 = (int)(fiz + 1)*sqSize + (int)(fiy + 1)*size + (int)fix + 0;
    int i111 = (int)(fiz + 1)*sqSize + (int)(fiy + 1)*size + (int)fix + 1;

    float restx = glm::mod(fix, 1.0f);
    float resty = glm::mod(fiy, 1.0f);
    float restz = glm::mod(fiz, 1.0f);

    float value_000 = map[i000];
    float value_001 = map[i001];
    float value_010 = map[i010];
    float value_011 = map[i011];

    float value_100 = map[i100];
    float value_101 = map[i101];
    float value_110 = map[i110];
    float value_111 = map[i111];

    float ma = glm::mix(value_000, value_001, restx);
    float mb = glm::mix(value_010, value_011, restx);

    float mc = glm::mix(value_100, value_101, restx);
    float md = glm::mix(value_110, value_111, restx);

    float me = glm::mix(ma, mb, resty);
    float mf = glm::mix(mc, md, resty);

    float mg = glm::mix(ma, mb, restz);

    return mg;
}
