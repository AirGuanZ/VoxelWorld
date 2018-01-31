/*================================================================
Filename: V1/LandGenerator.cpp
Date: 2018.1.30
Created by AirGuanZ
================================================================*/
#include <cassert>
#include <cstdlib>

#include "Biome.h"
#include "LandGenerator.h"

using namespace LandGenerator_V1;

void LandGenerator::GenerateLand(Chunk *ck)
{
    assert(ck != nullptr);
    auto [ckX, ckZ] = ck->GetPosition();
    int xBase = ck->GetXPosBase();
    int zBase = ck->GetZPosBase();

    BiomeGenerator biome(seed_);
    biome.Generate(ckX, ckZ);

    for(int x = 0; x != CHUNK_SECTION_SIZE; ++x)
    {
        for(int z = 0; z != CHUNK_SECTION_SIZE; ++z)
        {
            auto biomeRt = biome.GetResult(x, z);

            float baseHeight = biomeRt.factor * BaseHeight(biomeRt.type) +
                               biomeRt.neiFactor * BaseHeight(biomeRt.neiType);
            float variHeight = biomeRt.factor * VariHeight(biomeRt.type) +
                               biomeRt.neiFactor * VariHeight(biomeRt.neiType);

            int h = static_cast<int>(baseHeight + variHeight * Noise(xBase + x, zBase + z));
            switch(biomeRt.type)
            {
            case BiomeType::Ocean:
                MakeOcean(ck, x, z, h);
                break;
            case BiomeType::Field:
                MakeField(ck, x, z, h);
                break;
            case BiomeType::Plain:
                MakePlain(ck, x, z, h);
                break;
            case BiomeType::Desert:
                MakeDesert(ck, x, z, h);
                break;
            case BiomeType::Hill:
                MakeHill(ck, x, z, h);
                break;
            default:
                std::abort();
            }
        }
    }

    for(int x = 0; x != CHUNK_SECTION_SIZE; ++x)
    {
        for(int z = 0; z != CHUNK_SECTION_SIZE; ++z)
        {
            int H = ck->GetHeight(x, z);
            for(int y = 0; y <= H; ++y)
                ck->SetBlockLight(x, y, z, LIGHT_ALL_MIN);

            for(int y = H + 1; y < CHUNK_MAX_HEIGHT; ++y)
                ck->SetBlockLight(x, y, z, LIGHT_MIN_MIN_MIN_MAX);
        }
    }
}

float LandGenerator::Random(Seed seedOffset, int blkX, int blkZ, float min, float max) const
{
    return std::uniform_real_distribution<float>(min, max)(
        RandomEngine((seed_ + seedOffset) * blkX + blkZ));
}

float LandGenerator::Noise(int x, int z) const
{
    constexpr int LEVEL_NUM = 5;
    constexpr int MAX_GRID_SIZE = 128;
    constexpr float MAX_LEVEL_HEIGHT = 30.0f;

    int gridSize = MAX_GRID_SIZE;
    float levelHeight = MAX_LEVEL_HEIGHT;
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);

    auto randXZ = [&](int x, int z, int level)
    {
        return dis(RandomEngine(seed_ * x + z + level * level));
    };

    auto lerp = [](float a, float b, float t)
    {
        return a + (b - a) * (3 * t * t - 2 * t * t * t);
    };

    float result = 0.0f, base = 0.0f;
    for(int level = 0; level != LEVEL_NUM; ++level)
    {
        int gridX = (x + 0x40000000) / gridSize - 0x40000000 / gridSize;
        int gridZ = (z + 0x40000000) / gridSize - 0x40000000 / gridSize;

        float xzH = randXZ(gridX, gridZ, level);
        float x1zH = randXZ(gridX + 1, gridZ, level);
        float xz1H = randXZ(gridX, gridZ + 1, level);
        float x1z1H = randXZ(gridX + 1, gridZ + 1, level);
        float tX = static_cast<float>(x - gridX * gridSize) / gridSize;
        float tZ = static_cast<float>(z - gridZ * gridSize) / gridSize;

        result += levelHeight * lerp(lerp(xzH, x1zH, tX), lerp(xz1H, x1z1H, tX), tZ);
        base += levelHeight;
        gridSize /= 2;
        levelHeight *= 0.7f;
    }

    return result / base;
}

float LandGenerator::BaseHeight(BiomeType type) const
{
    switch(type)
    {
    case BiomeType::Ocean:
        return 25.0f;
    case BiomeType::Field:
        return 35.0f;
    case BiomeType::Plain:
        return 30.0f;
    case BiomeType::Desert:
        return 30.0f;
    case BiomeType::Hill:
        return 40.0f;
    }
    return 10.0f;
}

float LandGenerator::VariHeight(BiomeType type) const
{
    switch(type)
    {
    case BiomeType::Ocean:
        return 5.0f;
    case BiomeType::Field:
        return 10.0f;
    case BiomeType::Plain:
        return 3.0f;
    case BiomeType::Desert:
        return 10.0f;
    case BiomeType::Hill:
        return 30.0f;
    }
    return 10.0f;
}

void LandGenerator::MakeOcean(Chunk *ck, int x, int z, int h) const
{
    ck->SetBlockType(x, 0, z, BlockType::Bedrock);
    for(int y = 1; y < h; ++y)
        ck->SetBlockType(x, y, z, BlockType::Stone);
    ck->SetBlockType(x, h, z,
        Random(457, ck->GetXPosBase() + x, ck->GetZPosBase() + z, 0.0f, 1.0f) > 0.5f ?
        BlockType::Sand : BlockType::Stone);

    constexpr int WATER_LEVEL = 30;
    for(int y = h + 1; y <= WATER_LEVEL; ++y)
        ck->SetBlockType(x, y, z, BlockType::Water);
    h = (std::max)(h, WATER_LEVEL);

    for(int y = h + 1; y < CHUNK_MAX_HEIGHT; ++y)
        ck->SetBlockType(x, y, z, BlockType::Air);

    ck->SetHeight(x, z, h);
}

void LandGenerator::MakeField(Chunk *ck, int x, int z, int h) const
{
    ck->SetBlockType(x, 0, z, BlockType::Bedrock);
    for(int y = 1; y < h - 2; ++y)
        ck->SetBlockType(x, y, z, BlockType::Stone);
    for(int y = h - 2; y < h; ++y)
        ck->SetBlockType(x, y, z, BlockType::Dirt);
    ck->SetBlockType(x, h, z, BlockType::GrassBox);

    float plantRand = Random(213, ck->GetXPosBase() + x, ck->GetZPosBase() + z, 0.0f, 1.0f);
    if(plantRand < 0.002f)
        ck->SetBlockType(x, ++h, z, BlockType::Flower);
    else if(plantRand < 0.006f)
        ck->SetBlockType(x, ++h, z, BlockType::Grass);

    for(int y = h + 1; y < CHUNK_MAX_HEIGHT; ++y)
        ck->SetBlockType(x, y, z, BlockType::Air);

    ck->SetHeight(x, z, h);
}

void LandGenerator::MakePlain(Chunk *ck, int x, int z, int h) const
{
    MakeField(ck, x, z, h);
}

void LandGenerator::MakeDesert(Chunk *ck, int x, int z, int h) const
{
    ck->SetBlockType(x, 0, z, BlockType::Bedrock);
    for(int y = 1; y <= h; ++y)
        ck->SetBlockType(x, y, z, BlockType::Sand);

    for(int y = h + 1; y < CHUNK_MAX_HEIGHT; ++y)
        ck->SetBlockType(x, y, z, BlockType::Air);

    ck->SetHeight(x, z, h);
}

void LandGenerator::MakeHill(Chunk *ck, int x, int z, int h) const
{
    MakeField(ck, x, z, h);
}
