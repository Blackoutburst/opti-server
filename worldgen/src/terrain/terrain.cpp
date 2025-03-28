#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtc/random.hpp>

#include "common.hpp"
#include "generationStage.hpp"
#include "terrain/terrain.hpp"
#include "utils/easings.hpp"
#include "utils/threadsafe_random.hpp"

static GridData<CHUNK_SIZE, 1> getTerrainTotalDensity(const glm::ivec3& chunkWorldPosition)
{
    auto data = std::make_shared<float[]>(CHUNK_BLOCK_COUNT);
    float* p = data.get();

    GridData v_continental = noise_continental.genGrid2D(chunkWorldPosition);
    GridData v_terrain_density = noise_terrain_density.genGrid3D(chunkWorldPosition);

    for (int dz = 0 ; dz < CHUNK_SIZE ; ++dz) {
    for (int dx = 0 ; dx < CHUNK_SIZE ; ++dx) {
        float height = Easings::easeInOutQuint(v_continental.get(dx, dz));
        float heightW = height * 200.0f + 16;

        for (int dy = 0 ; dy < CHUNK_SIZE ; ++dy) {
            float terrainDensity = v_terrain_density.get(dx, dy, dz);
            float density = heightW + terrainDensity * 70.0f;// mapRange(terrainDensity, 0, 64, 0.0f, 0.5f);
            int i = INDEX_XYZ(dx, dy, dz);

            p[i] = density;
        }
    }}

    // density = height * 200.0f + 16 + terrainDensity * 70.0f
    // density = height * 200.0f + terrainDensity * 70.0f
    // density += 16
    // if (density > worldY) {}

    return GridData<CHUNK_SIZE, 1>(data);
}

static void generateNether(uint8_t* blocks, const glm::ivec3& chunkWorldPosition)
{
    GridData n_terrain_density = noise_nether_density.genGrid3D(chunkWorldPosition);

    threadSafeRandomGenerator::seed(chunkWorldPosition.x ^ chunkWorldPosition.y ^ chunkWorldPosition.z);


    for (int dz = 0 ; dz < CHUNK_SIZE ; ++dz) {
    for (int dy = 0 ; dy < CHUNK_SIZE ; ++dy) {
    for (int dx = 0 ; dx < CHUNK_SIZE ; ++dx) {
        int i = INDEX_XYZ(dx, dy, dz);

        float world_y = chunkWorldPosition.y + dy;

        float x = mapRange(world_y, -512, -256, -1.0f, 1.0f);

        float d = n_terrain_density.get(dx, dy, dz) * 0.5f + 0.5f;
        float density = d * (1.0f - x*x); // bell curve

        if (density < 0.4f) {
            constexpr int TRANSITION_HEIGHT = 5;
            if (world_y > -256 - TRANSITION_HEIGHT) { // transition
                float t = mapRange(world_y, -256 - TRANSITION_HEIGHT, -256, 0.0f, 1.0f);
                float rd = threadSafeRandomGenerator::randLinear(0.0f, 1.0f);
                blocks[i] = rd > t ? (uint8_t)BlockType::Netherrack : (uint8_t)BlockType::Stone;
            } else {
                blocks[i] = (uint8_t)BlockType::Netherrack;
            }

        } else {
            if (world_y < -450) {
                blocks[i] = (uint8_t)BlockType::Lava;
            } else {
                blocks[i] = (uint8_t)BlockType::Air;
            }
        }

    }}}
}

static void generateOverworld(uint8_t* blocks, const glm::ivec3& chunkWorldPosition)
{
    GridData terrain_density = getTerrainTotalDensity(chunkWorldPosition);

    for (int dz = 0 ; dz < CHUNK_SIZE ; ++dz) {
    for (int dx = 0 ; dx < CHUNK_SIZE ; ++dx) {
        for (int dy = 0 ; dy < CHUNK_SIZE ; ++dy) {
            int i = INDEX_XYZ(dx, dy, dz);
            float worldY = chunkWorldPosition.y + dy;
            float density = terrain_density[i];

            if (density > worldY) {
                blocks[i] = (uint8_t)BlockType::Stone;
            } else {
                if (worldY < 0) {
                    blocks[i] = (uint8_t)BlockType::Water;
                } else {
                    blocks[i] = (uint8_t)BlockType::Air;
                }
            }

        }
    }}
}

void generateTerrain(uint8_t* blocks, const glm::ivec3& chunkWorldPosition) {
    if (chunkWorldPosition.y > -256) {
        generateOverworld(blocks, chunkWorldPosition);
    } else {
        generateNether(blocks, chunkWorldPosition);
    }
}

// glm::vec3 getSurfaceGradient(const float* density, int x, int y, int z)
// {
//     glm::vec3 gradient;

//     if (x == CHUNK_SIZE-1 || y == CHUNK_SIZE-1 || z == CHUNK_SIZE-1) return {0, 1, 0};

//     int i000 = INDEX_XYZ(x, y, z);
//     int i100 = INDEX_XYZ(x+1, y, z);
//     int i010 = INDEX_XYZ(x, y+1, z);
//     int i001 = INDEX_XYZ(x, y, z+1);

//     gradient.x = density[i000] - density[i100];
//     gradient.y = density[i000] - density[i010];
//     gradient.z = density[i000] - density[i001];

//     gradient = glm::normalize(gradient);

//     return gradient;
// }

void generateSurface(uint8_t* blocks, const glm::ivec3& chunkWorldPosition) {
    if (chunkWorldPosition.y < -256) return;

    uint8_t top_chunk[CHUNK_BLOCK_COUNT];
    generateStages<GenerationStage::Terrain>(top_chunk, chunkWorldPosition + glm::ivec3(0, CHUNK_SIZE, 0));

    // GridData density = getTerrainTotalDensity(chunkWorldPosition);
    // Cannot use density because it isn't normalized

    // top layer
    for (int dz = 0 ; dz < CHUNK_SIZE ; ++dz) {
    for (int dx = 0 ; dx < CHUNK_SIZE ; ++dx) {
        int i0 = INDEX_XYZ(dx, CHUNK_SIZE-1, dz);
        int i1 = INDEX_XYZ(dx, 0, dz);

        if (blocks[i0] == (uint8_t)BlockType::Stone && top_chunk[i1] == (uint8_t)BlockType::Air) {
            blocks[i0] = (uint8_t)BlockType::Grass;
        }
    }}

    for (int dz = 0 ; dz < CHUNK_SIZE ; ++dz) {
    for (int dx = 0 ; dx < CHUNK_SIZE ; ++dx) {
    for (int dy = 0 ; dy < CHUNK_SIZE-1 ; ++dy) {
        int i0 = INDEX_XYZ(dx, dy, dz);
        int i1 = INDEX_XYZ(dx, dy+1, dz);

        // glm::vec3 g = getSurfaceGradient(density.data(), dx, dy, dz);
        // if (!(g.y > 0.4f)) continue;

        if (blocks[i0] == (uint8_t)BlockType::Stone && blocks[i1] == (uint8_t)BlockType::Air) {
            blocks[i0] = (uint8_t)BlockType::Grass;

            for (int i = 0 ; i < 3 ; ++i) {
                if (setBlock(blocks, (uint8_t)BlockType::Dirt, dx, dy-1-i, dz) == 0) break;
            }
        }
    }}}
}


// https://iquilezles.org/articles/smin/
// quadratic polynomial (faster)
float smin( float a, float b, float k )
{
    k *= 4.0;
    float h = glm::max( k-glm::abs(a-b), 0.0f )/k;
    return glm::min(a,b) - h*h*k*(1.0f/4.0f);
}

void generateCaves(uint8_t* blocks, const glm::ivec3& chunkWorldPosition) {
    if (chunkWorldPosition.y < -256-16) return;

    GridData v_continental = noise_continental.genGrid2D(chunkWorldPosition);
    GridData v_caveDensity = noise_cave_density.genGrid3D(chunkWorldPosition, 0.0085f);
    GridData v_caveBig = noise_cave_density1.genGrid3D(chunkWorldPosition, 0.006f);

    for (int z = 0 ; z < CHUNK_SIZE ; ++z) {
    for (int x = 0 ; x < CHUNK_SIZE ; ++x) {
        float height = v_continental.get(x, z) * 200.0f + 32;
        for (int y = 0 ; y < CHUNK_SIZE ; ++y) {

            int index = INDEX_XYZ(x, y, z);
            float world_y = chunkWorldPosition.y + y;

            if (world_y > height) continue;

            float minFactor = 0.9f;
            float maxFactor = 1.0f;
            float threshold = glm::max(minFactor, mapRange(world_y, -64, height, minFactor, maxFactor));

            float density = v_caveDensity.get(x, y, z);
            float densityBig = v_caveBig.get(x, y, z) * 0.5f + 0.5f;

            density = 1.0f - smin(1.0f - density, 1.0f - (float)(densityBig < 0.3f), 0.05f); // smooth max du pauvre

            if (world_y <= -256) { // Nether transition zone between -256 and -256-16
                float t = mapRange(world_y, -256-16, -256, 1.0f, 0.0f);
                threshold = glm::mix(threshold, 1.0f, t);
            }

            if (density > threshold) {
                if (blocks[index] != (uint8_t)BlockType::Water) {
                    blocks[index] = (uint8_t)BlockType::Air;
                }
            }
        }
    }}
}
