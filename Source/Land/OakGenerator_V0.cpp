/*================================================================
Filename: OakGenerator_V0.cpp
Date: 2018.1.26
Created by AirGuanZ
================================================================*/
#include <cassert>

#include "OakGenerator_V0.h"

OakGenerator_V0::OakGenerator_V0(Seed seed)
    : seed_(seed)
{

}

float OakGenerator_V0::Random(Seed seedOffset, int blkX, int blkZ, float min, float max) const
{
    return std::uniform_real_distribution<float>(min, max)(
        RandomEngine((seed_ + seedOffset) * blkX + blkZ));
}

void OakGenerator_V0::Try(Chunk *ck, int x, int y, int z, std::vector<IntVector3> &lightUpdates) const
{
    assert(ck != nullptr);
    
    Chunk::BlockTypeData &blks = ck->blocks;
    Chunk::HeightMap &hm = ck->heightMap;

    //放置橡树的合法性检验

    if(((x - 3) | (CHUNK_SECTION_SIZE - 1 - (x + 3)) |
        (z - 3) | (CHUNK_SECTION_SIZE - 1 - (z + 3)) |
         y      | (CHUNK_MAX_HEIGHT - 1 - (y + 7))) < 0)
        return;

    for(int h = y + 1; h <= y + 4; ++h)
    {
        if(ck->GetBlockType(x, h, z) != BlockType::Air)
            return;
    }

    for(int dx = x - 3; dx <= x + 3; ++dx)
    {
        for(int dz = z - 3; dz <= z + 3; ++dz)
        {
            for(int h = y + 5; h <= y + 7; ++h)
            {
                if(ck->GetBlockType(dx, h, dz) != BlockType::Air)
                    return;
            }
        }
    }

    //放置橡树方块

    int xBase = ck->GetXPosBase();
    int zBase = ck->GetZPosBase();

    for(int h = y + 1; h <= y + 6; ++h)
        ck->SetBlockType(x, h, z, BlockType::Wood);

    for(int dx = x - 3; dx <= x + 3; ++dx)
    {
        for(int dz = z - 3; dz <= z + 3; ++dz)
        {
            if(dx != x || dz != z)
            {
                ck->SetBlockType(dx, y + 5, dz, BlockType::Leaf);
                /*for(int H = ck->GetHeight(dx, dz); H < y + 5; ++H)
                    lightUpdates.push_back({ xBase + dx, H, zBase + dz });*/
                ck->SetHeight(dx, dz, y + 5);
            }
        }
    }

    for(int dx = x - 2; dx <= x + 2; ++dx)
    {
        for(int dz = z - 2; dz <= z + 2; ++dz)
        {
            if(dx != x || dz != z)
            {
                ck->SetBlockType(dx, y + 6, dz, BlockType::Leaf);
                ck->SetHeight(dx, dz, y + 6);
            }
        }
    }

    for(int dx = x - 1; dx <= x + 1; ++dx)
    {
        for(int dz = z - 1; dz <= z + 1; ++dz)
        {
            ck->SetBlockType(dx, y + 7, dz, BlockType::Leaf);
            ck->SetHeight(dx, dz, y + 7);
        }
    }

    //设置光照更新列表

    /*for(int dx = x - 3; dx <= x + 3; ++dx)
    {
        for(int dz = z - 3; dz <= z + 3; ++dz)
        {
            int mdis = (std::max)(std::abs(dx - x), std::abs(dz - z));
            if(mdis <= 1)
                lightUpdates.push_back({ xBase + dx, y + 7, zBase + dz });
            else if(mdis <= 2)
                lightUpdates.push_back({ xBase + dx, y + 6, zBase + dz });
            else
                lightUpdates.push_back({ xBase + dx, y + 5, zBase + dz });
        }
    }*/
}

void OakGenerator_V0::Make(Chunk *ck, std::vector<IntVector3> &lightUpdates) const
{
    assert(ck != nullptr);
    
    Chunk::HeightMap &hm = ck->heightMap;

    int xBase = ck->GetXPosBase();
    int zBase = ck->GetZPosBase();

    for(int x = 3; x <= CHUNK_SECTION_SIZE - 1 - 3; ++x)
    {
        for(int z = 3; z <= CHUNK_SECTION_SIZE - 1 - 3; ++z)
        {
            if(Random(0xAB, xBase + x, zBase + z, 0.0f, 1.0f) < 0.005f &&
               ck->GetBlockType(x, ck->GetHeight(x, z), z) == BlockType::GrassBox)
                Try(ck, x, ck->GetHeight(x, z), z, lightUpdates);
        }
    }
}
