#pragma once

namespace threadSafeRandomGenerator {
    void seed(int seed);
    float rand();
    float randLinear(float min, float max);
}
