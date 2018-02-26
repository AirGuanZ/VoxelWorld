/*================================================================
Filename: V2/Biome_V2.h
Date: 2018.2.3
Created by AirGuanZ
================================================================*/
#pragma once

#include <cassert>

#include <Chunk/Chunk.h>
#include "Area_V2.h"
#include "Common_V2.h"

namespace LandGenerator_V2
{
    class Biome
    {
    public:
        Biome(Seed seed)
            : seed_(seed)
        {

        }

        float BaseHeight(AreaType type) const
        {
            switch(type)
            {
            case AreaType::Normal:
                return 2.0f;
            case AreaType::Desert:
                return 2.0f;
            }
            return 10.0f;
        }
        float VariHeight(AreaType type) const
        {
            switch(type)
            {
            case AreaType::Normal:
                return 80.0f;
            case AreaType::Desert:
                return 20.0f;
            }
            return 10.0f;
        }

        void SetBlockColumn(Chunk *ck, AreaType type, int x, int z, int h) const
        {
            switch(type)
            {
            case AreaType::Normal:
                SetNormalBlockColumn(ck, x, z, h);
                break;
            case AreaType::Desert:
                SetDesertBlockColumn(ck, x, z, h);
                break;
            default:
                std::abort();
            }
        }
        
    private:
        float Rand(Seed seedOffset, int blkX, int blkZ, float min, float max) const
        {
            return std::uniform_real_distribution<float>(min, max)(
                std::mt19937_64((seed_ + seedOffset) * blkX + blkZ));
        }

        void SetNormalBlockColumn(Chunk *ck, int x, int z, int h) const
        {
            assert(h > 0);

            ck->SetBlockType(x, 0, z, BlockType::Bedrock);
            for(int y = 1; y != h - 2; ++y)
                ck->SetBlockType(x, y, z, BlockType::Stone);

            for(int y = h - 2; y != h; ++y)
                ck->SetBlockType(x, y, z, BlockType::Dirt);

            ck->SetBlockType(x, h, z, BlockType::GrassBox);

            float gfv = Rand(13, x + ck->GetXPosBase(), z + ck->GetZPosBase(), 0.0f, 1.0f);
            if(gfv < 0.1f)
                ck->SetBlockType(x, ++h, z, BlockType::Grass);
            else if(gfv < 0.102f)
                ck->SetBlockType(x, ++h, z, BlockType::Flower);

            for(int y = h + 1; y != CHUNK_MAX_HEIGHT; ++y)
                ck->SetBlockType(x, y, z, BlockType::Air);

            ck->SetHeight(x, z, h);
        }

        void SetDesertBlockColumn(Chunk *ck, int x, int z, int h) const
        {
            assert(h > 0);

            ck->SetBlockType(x, 0, z, BlockType::Bedrock);
            for(int y = 1; y != h - 4; ++y)
                ck->SetBlockType(x, y, z, BlockType::Stone);

            for(int y = h - 4; y <= h; ++y)
                ck->SetBlockType(x, y, z, BlockType::Sand);

            float gfv = Rand(13, x + ck->GetXPosBase(), z + ck->GetZPosBase(), 0.0f, 1.0f);
            if(gfv < 0.005f)
                ck->SetBlockType(x, ++h, z, BlockType::DriedGrass);

            for(int y = h + 1; y != CHUNK_MAX_HEIGHT; ++y)
                ck->SetBlockType(x, y, z, BlockType::Air);

            ck->SetHeight(x, z, h);
        }

    private:
        Seed seed_;
    };
}
