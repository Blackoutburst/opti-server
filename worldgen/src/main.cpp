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
    fract_terrain->SetOctaveCount(3);
    fract_terrain->SetGain(0.5f);
    fract_terrain->SetLacunarity(2.0f);
    fract_terrain->SetWeightedStrength(0.5f);

    fn_terrain = fract_terrain;
    // -- //


    // Continental //
    auto f1 = FastNoise::New<FastNoise::Simplex>();
    auto f1_scale = FastNoise::New<FastNoise::DomainAxisScale>();
    f1_scale->SetSource(f1);
    f1_scale->SetScale<FastNoise::Dim::X>(0.0005f);
    f1_scale->SetScale<FastNoise::Dim::Y>(0.0005f);
    f1_scale->SetScale<FastNoise::Dim::Z>(0.0005f);

    auto f1_scale_fract = FastNoise::New<FastNoise::FractalFBm>();
    f1_scale_fract->SetSource(f1_scale);
    f1_scale_fract->SetOctaveCount(3);
    f1_scale_fract->SetGain(0.5f);
    f1_scale_fract->SetLacunarity(2.0f);
    f1_scale_fract->SetWeightedStrength(0.5f);

    noise_continental = f1_scale_fract;
    // -- //

    // Terrain density //
    auto f2 = FastNoise::New<FastNoise::Perlin>();
    // auto f2 = FastNoise::New<FastNoise::CellularDistance>();
    // f2->SetReturnType(FastNoise::CellularDistance::ReturnType::Index0Mul1);
    // f2->SetDistanceFunction(FastNoise::DistanceFunction::EuclideanSquared);

    auto f2_fract = FastNoise::New<FastNoise::FractalFBm>();
    f2_fract->SetSource(f2);
    f2_fract->SetOctaveCount(3);
    f2_fract->SetGain(0.5f);
    f2_fract->SetLacunarity(2.0f);
    f2_fract->SetWeightedStrength(0.5f);

    auto f2_warp = FastNoise::New<FastNoise::DomainWarpGradient>();
    f2_warp->SetSource(f2_fract);
    f2_warp->SetWarpAmplitude(0.02f);
    f2_warp->SetWarpFrequency(16.0f);

    auto f2_scale = FastNoise::New<FastNoise::DomainAxisScale>();
    f2_scale->SetSource(f2_warp);
    f2_scale->SetScale<FastNoise::Dim::X>(0.01f);
    f2_scale->SetScale<FastNoise::Dim::Y>(0.01f);
    f2_scale->SetScale<FastNoise::Dim::Z>(0.01f);

    noise_terrain_density = f2_scale;
    // -- //
}

void genChunk(uint8_t* blocks, int32_t x, int32_t y, int32_t z) {
    const glm::ivec3 chunkWorldPos = {x, y, z};

    generateStage1(blocks, chunkWorldPos);
    generateStageSurface(blocks, chunkWorldPos);
    // generateStage2(blocks, chunkWorldPos);
    generateTrees(blocks, chunkWorldPos);
}
