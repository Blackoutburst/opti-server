#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "utils/easings.hpp"

namespace Easings
{
    float easeInSine(float x) {
        return 1.0f - glm::cos((x * glm::pi<float>()) / 2.0f);
    }

    float easeOutSine(float x) {
        return glm::sin((x * glm::pi<float>()) / 2.0f);
    }

    float easeInOutSine(float x) {
        return -(glm::cos(glm::pi<float>() * x) - 1.0f) / 2.0f;
    }

    float easeInQuad(float x) {
        return x * x;
    }

    float easeOutQuad(float x) {
        return 1.0f - (1.0f - x) * (1.0f - x);
    }

    float easeInOutQuad(float x) {
        return x < 0.5f ? 2.0f * x * x : 1.0f - glm::pow(-2.0f * x + 2.0f, 2.0f) / 2.0f;
    }

    float easeInCubic(float x) {
        return x * x * x;
    }

    float easeOutCubic(float x) {
        return 1.0f - glm::pow(1.0f - x, 3.0f);
    }

    float easeInOutCubic(float x) {
        return x < 0.5f ? 4.0f * x * x * x : 1.0f - glm::pow(-2.0f * x + 2.0f, 3.0f) / 2.0f;
    }

    float easeInQuart(float x) {
        return x * x * x * x;
    }

    float easeOutQuart(float x) {
        return 1.0f - glm::pow(1.0f - x, 4.0f);
    }

    float easeInOutQuart(float x) {
        return x < 0.5f ? 8.0f * x * x * x * x : 1.0f - glm::pow(-2.0f * x + 2.0f, 4.0f) / 2.0f;
    }

    float easeInQuint(float x) {
        return x * x * x * x * x;
    }

    float easeOutQuint(float x) {
        return 1.0f - glm::pow(1.0f - x, 5.0f);
    }

    float easeInOutQuint(float x) {
        return x < 0.5f ? 16.0f * x * x * x * x * x : 1.0f - glm::pow(-2.0f * x + 2.0f, 5.0f) / 2.0f;
    }

    float easeInExpo(float x) {
        return x == 0.0f ? 0.0f : glm::pow(2.0f, 10.0f * x - 10.0f);
    }

    float easeOutExpo(float x) {
        return x == 1.0f ? 1.0f : 1.0f - glm::pow(2.0f, -10.0f * x);
    }

    float easeInOutExpo(float x) {
        return x == 0.0f
        ? 0.0f
        : x == 1.0f
        ? 1.0f
        : x < 0.5f ? glm::pow(2.0f, 20.0f * x - 10.0f) / 2.0f
        : (2.0f - glm::pow(2.0f, -20.0f * x + 10.0f)) / 2.0f;
    }

    float easeInCirc(float x) {
        return 1.0f - glm::sqrt(1.0f - glm::pow(x, 2.0f));
    }

    float easeOutCirc(float x) {
        return glm::sqrt(1.0f - glm::pow(x - 1.0f, 2.0f));
    }

    float easeInOutCirc(float x) {
        return x < 0.5f
        ? (1.0f - glm::sqrt(1.0f - glm::pow(2.0f * x, 2.0f))) / 2.0f
        : (glm::sqrt(1.0f - glm::pow(-2.0f * x + 2.0f, 2.0f)) + 1.0f) / 2.0f;
    }
}
