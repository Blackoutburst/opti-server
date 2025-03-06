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

    // Cave //
    FastNoise::SmartNode<FastNoise::CellularDistance> c = FastNoise::New<FastNoise::CellularDistance>();
    c->SetReturnType(FastNoise::CellularDistance::ReturnType::Index0Div1);
    c->SetDistanceFunction(FastNoise::DistanceFunction::EuclideanSquared);
    c->SetDistanceIndex0(1);
    c->SetDistanceIndex1(3);

    auto domainWarp = FastNoise::New<FastNoise::DomainWarpGradient>();
    domainWarp->SetSource(c);
    domainWarp->SetWarpAmplitude(0.2f);
    domainWarp->SetWarpFrequency(4.0f);

    auto domainScale = FastNoise::New<FastNoise::DomainAxisScale>();
    domainScale->SetSource(domainWarp);
    domainScale->SetScale<FastNoise::Dim::Y>(1.4f);

    fn_celullarDist = domainScale;
    // -- //

    // auto terrain = FastNoise::New<FastNoise::Perlin>();
    auto terrain = FastNoise::New<FastNoise::CellularDistance>();
    terrain->SetReturnType(FastNoise::CellularDistance::ReturnType::Index0Mul1);
    terrain->SetDistanceFunction(FastNoise::DistanceFunction::EuclideanSquared);

    auto warp_terrain = FastNoise::New<FastNoise::DomainWarpGradient>();
    warp_terrain->SetSource(terrain);
    warp_terrain->SetWarpAmplitude(0.02f);
    warp_terrain->SetWarpFrequency(16.0f);

    auto fract_terrain = FastNoise::New<FastNoise::FractalFBm>();
    fract_terrain->SetSource(warp_terrain);
    fract_terrain->SetOctaveCount(2);
    fract_terrain->SetGain(0.5f);
    fract_terrain->SetLacunarity(2.0f);
    fract_terrain->SetWeightedStrength(0.5f);

    fn_terrain = fract_terrain;
}

void genChunk(uint8_t* blocks, int32_t x, int32_t y, int32_t z) {
    const glm::ivec3 chunkWorldPos = {x, y, z};

    generateStage1(blocks, chunkWorldPos);
    // generateStage2(blocks, chunkWorldPos);

    // generateTrees(blocks, chunkWorldPos);
}
