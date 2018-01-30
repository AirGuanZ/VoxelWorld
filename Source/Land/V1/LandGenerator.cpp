/*================================================================
Filename: V1/LandGenerator.cpp
Date: 2018.1.30
Created by AirGuanZ
================================================================*/
#include <cassert>

#include "Biome.h"
#include "LandGenerator.h"

using namespace LandGenerator_V1;

void LandGenerator::GenerateLand(Chunk *ck)
{
    assert(ck != nullptr);
    auto [ckX, ckZ] = ck->GetPosition();

    BiomeGenerator biome(seed_);
    biome.Generate(ckX, ckZ);

    for(int x = 0; x != CHUNK_SECTION_SIZE; ++x)
    {
        for(int z = 0; z != CHUNK_SECTION_SIZE; ++z)
        {
            int h = 3 * static_cast<int>(biome.GetResult(x, z).type) + 20;
            ck->SetHeight(x, z, h);

            BlockType mainType = BlockType::Bedrock;
            switch(biome.GetResult(x, z).type)
            {
            case BiomeType::Ocean:
                mainType = BlockType::Water;
                break;
            case BiomeType::Plain:
                mainType = BlockType::GrassBox;
                break;
            case BiomeType::Hill:
                mainType = BlockType::Stone;
                break;
            case BiomeType::Field:
                mainType = BlockType::Dirt;
                break;
            case BiomeType::Desert:
                mainType = BlockType::Sand;
                break;
            }

            ck->SetBlockType(x, 0, z, BlockType::Bedrock);
            for(int y = 1; y <= h; ++y)
                ck->SetBlockType(x, y, z, mainType);
            for(int y = h + 1; y < CHUNK_MAX_HEIGHT; ++y)
                ck->SetBlockType(x, y, z, BlockType::Air);
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
