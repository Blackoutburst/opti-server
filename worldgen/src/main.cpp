#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <thread>

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include "glm/gtx/hash.hpp"
#include "glm/gtc/random.hpp"

#include "main.hpp"
#include "common.hpp"
#include "terrain/terrain.hpp"
#include "structures/vegetation.hpp"

void init()
{
    // fn = FastNoise::New<FastNoise::Perlin>();
    fn = FastNoise::New<FastNoise::Simplex>();
    fn_celullarValue = FastNoise::New<FastNoise::CellularValue>();

    FastNoise::SmartNode<FastNoise::CellularDistance> c = FastNoise::New<FastNoise::CellularDistance>();
    c->SetReturnType(FastNoise::CellularDistance::ReturnType::Index0Div1);
    c->SetDistanceFunction(FastNoise::DistanceFunction::EuclideanSquared);
    c->SetDistanceIndex0(1);
    c->SetDistanceIndex1(3);

    auto domainWarp = FastNoise::New<FastNoise::DomainWarpGradient>();
    domainWarp->SetSource(c);
    domainWarp->SetWarpAmplitude(0.2f);
    domainWarp->SetWarpFrequency(4.0f);

    fn_celullarDist = domainWarp;
}

void genChunk(uint8_t* blocks, int32_t x, int32_t y, int32_t z) {
    const glm::ivec3 chunkWorldPos = {x, y, z};

    generateStage1(blocks, chunkWorldPos);
    generateStage2(blocks, chunkWorldPos);

    generateTrees(blocks, chunkWorldPos);
}
