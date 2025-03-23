#include <random>

static thread_local std::mt19937 random_generator(0);
static thread_local std::uniform_real_distribution<> dis(0.0f, 1.0f);

namespace threadSafeRandomGenerator {
    void seed(int seed) {
        random_generator.seed(seed);
    }

    float rand() {
        return dis(random_generator);
    }

    float randLinear(float min, float max) {
        return min + dis(random_generator) * (max - min);
    }
}
