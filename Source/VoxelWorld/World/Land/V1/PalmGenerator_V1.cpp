/*================================================================
Filename: V1/Palm.cpp
Date: 2018.2.2
Created by AirGuanZ
================================================================*/
#include "PalmGenerator_V1.h"

using namespace LandGenerator_V1;

PalmGenerator::PalmGenerator(Seed seed)
    : seed_(seed)
{

}

float PalmGenerator::Random(Seed seedOffset, int blkX, int blkZ, float min, float max) const
{
    return std::uniform_real_distribution<float>(min, max)(
        RandomEngine((seed_ + seedOffset) * blkX + blkZ));
}

void PalmGenerator::Try(Chunk *ck, int x, int y, int z) const
{
    assert(ck != nullptr);

    if(((x - 3) | (CHUNK_SECTION_SIZE - 1 - (x + 3)) |
        (z - 3) | (CHUNK_SECTION_SIZE - 1 - (z + 3)) |
        y | (CHUNK_MAX_HEIGHT - 1 - (y + 7))) < 0)
        return;

    for(int h = y + 1; h <= y + 7; ++h)
    {
        if(ck->GetBlockType(x, h, z) != BlockType::Air)
            return;
    }

    for(int dx = x - 3; dx <= x + 3; ++dx)
    {
        for(int dz = z - 3; dz <= z + 3; ++dz)
        {
            if(ck->GetBlockType(dx, y + 5, dz) != BlockType::Air)
                return;
            if(ck->GetBlockType(dx, y + 6, dz) != BlockType::Air)
                return;
        }
    }

    int xBase = ck->GetXPosBase();
    int zBase = ck->GetZPosBase();

    for(int h = y + 1; h <= y + 6; ++h)
        ck->SetBlockType(x, h, z, BlockType::Wood);
    ck->SetBlockType(x, y + 7, z, BlockType::Leaf);
    ck->SetHeight(x, z, y + 7);

    ck->SetBlockType(x - 3, y + 5, z, BlockType::Leaf);
    for(int dx = x - 3; dx < x; ++dx)
    {
        ck->SetBlockType(dx, y + 6, z, BlockType::Leaf);
        ck->SetHeight(dx, z, y + 6);
    }

    ck->SetBlockType(x + 3, y + 5, z, BlockType::Leaf);
    for(int dx = x + 1; dx <= x + 3; ++dx)
    {
        ck->SetBlockType(dx, y + 6, z, BlockType::Leaf);
        ck->SetHeight(dx, z, y + 6);
    }

    ck->SetBlockType(x, y + 5, z - 3, BlockType::Leaf);
    for(int dz = z - 3; dz < z; ++dz)
    {
        ck->SetBlockType(x, y + 6, dz, BlockType::Leaf);
        ck->SetHeight(x, dz, y + 6);
    }

    ck->SetBlockType(x, y + 5, z + 3, BlockType::Leaf);
    for(int dz = z + 1; dz <= z + 3; ++dz)
    {
        ck->SetBlockType(x, y + 6, dz, BlockType::Leaf);
        ck->SetHeight(x, dz, y + 6);
    }
}

void PalmGenerator::Make(Chunk *ck, const BiomeGenerator &biome) const
{
    assert(ck != nullptr);

    int xBase = ck->GetXPosBase();
    int zBase = ck->GetZPosBase();

    for(int x = 3; x <= CHUNK_SECTION_SIZE - 1 - 3; ++x)
    {
        for(int z = 3; z <= CHUNK_SECTION_SIZE - 1 - 3; ++z)
        {
            if(Random(14727, xBase + x, zBase + z, 0.0f, 1.0f) < 0.005f &&
                ck->GetBlockType(x, ck->GetHeight(x, z), z) == BlockType::Sand &&
                biome.GetResult(x, z).type == BiomeType::Ocean)
                Try(ck, x, ck->GetHeight(x, z), z);
        }
    }
}
