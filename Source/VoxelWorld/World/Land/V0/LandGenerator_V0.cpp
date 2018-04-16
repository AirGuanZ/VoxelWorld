/*================================================================
Filename: LandGenerator_V0.cpp
Date: 2018.1.21
Created by AirGuanZ
================================================================*/
#include <World\Chunk\Chunk.h>
#include <World\Chunk\ChunkTraversal.h>
#include <World\Land\PerlinNoise\PerlinNoise2D.h>
#include "LandGenerator_V0.h"
#include "OakGenerator_V0.h"

::LandGenerator_V0::LandGenerator::LandGenerator(Seed seed)
    : seed_(seed)
{

}

void ::LandGenerator_V0::LandGenerator::GenerateLand(Chunk *ck)
{
    assert(ck != nullptr);

    Chunk::BlockTypeData &data = ck->blocks;
    Chunk::HeightMap &heightMap = ck->heightMap;
    Chunk::BlockLightData &lights = ck->lights;

    IntVectorXZ ckPos = ck->GetPosition();
    int xBase = ChunkXZ_To_BlockXZ(ckPos.x);
    int zBase = ChunkXZ_To_BlockXZ(ckPos.z);
    for(int x = 0; x != CHUNK_SECTION_SIZE; ++x)
    {
        for(int z = 0; z != CHUNK_SECTION_SIZE; ++z)
        {
            int h = GetHeight(x + xBase, z + zBase);

            ck->SetBlockType(x, 0, z, BlockType::Bedrock);

            for(int y = 1; y != h - 2; ++y)
                ck->SetBlockType(x, y, z, BlockType::Stone);

            for(int y = h - 2; y != h; ++y)
                ck->SetBlockType(x, y, z, BlockType::Dirt);

            constexpr int WATER_LEVEL = 50;
            if(h < WATER_LEVEL)
            {
                for(int y = h; y <= WATER_LEVEL; ++y)
                    ck->SetBlockType(x, y, z, BlockType::Water);
                h = WATER_LEVEL;
            }
            else
            {
                ck->SetBlockType(x, h, z, BlockType::GrassBox);

                float gfv = Random(1, x + xBase, z + zBase, 0.0f, 1.0f);
                if(gfv < 0.1f)
                    ck->SetBlockType(x, ++h, z, BlockType::Grass);
                else if(gfv < 0.11f)
                    ck->SetBlockType(x, ++h, z, BlockType::Flower);
            }

            for(int y = h + 1; y != CHUNK_MAX_HEIGHT; ++y)
                ck->SetBlockType(x, y, z, BlockType::Air);

            ck->SetHeight(x, z, h);
        }
    }

    //Éú³ÉÊ÷
    OakGenerator(seed_).Make(ck);

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

int ::LandGenerator_V0::LandGenerator::GetHeight(int x, int z)
{
    return static_cast<int>(BasicPerlinNoise2D<>::Gen(seed_, x, z, 4, 10.0f, 100.0f));
}

float ::LandGenerator_V0::LandGenerator::Random(Seed seedOffset, int blkX, int blkZ, float min, float max)
{
    return std::uniform_real_distribution<float>(min, max)(
        RandomEngine((seed_ + seedOffset) * blkX + blkZ));
}
