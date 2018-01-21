/*================================================================
Filename: LandGenerator.cpp
Date: 2018.1.21
Created by AirGuanZ
================================================================*/
#include "LandGenerator.h"

TestLandGenerator TestLandGenerator::instance_;

void TestLandGenerator::GenerateLand(Chunk *ck)
{
    //把Chunk数据下半部分设为Stone，上半部分设为Air
    Chunk::BlockData &data = ck->GetBlockData();
    for(int x = 0; x != CHUNK_SECTION_SIZE; ++x)
    {
        for(int z = 0; z != CHUNK_SECTION_SIZE; ++z)
        {
            for(int y = 0; y != CHUNK_MAX_HEIGHT / 2; ++y)
            {
                Block blk;
                blk.type = BlockType::Grass;
                blk.sunlight = 1.0f;
                blk.lightColor = { 0, 0, 0 };
                data[x][y][z] = blk;
            }

            for(int y = CHUNK_MAX_HEIGHT / 2; y != CHUNK_MAX_HEIGHT; ++y)
            {
                Block blk;
                blk.type = BlockType::Air;
                blk.sunlight = 1.0f;
                blk.lightColor = { 0, 0, 0 };
                data[x][y][z] = blk;
            }
        }
    }

    if(ck->GetPosition().x == 0)
    {
        for(int x = 0; x != CHUNK_SECTION_SIZE; ++x)
        {
            for(int z = 0; z != CHUNK_SECTION_SIZE; ++z)
            {
                for(int y = CHUNK_MAX_HEIGHT / 2; y != CHUNK_MAX_HEIGHT / 2 + 10; ++y)
                {
                    Block blk;
                    blk.type = BlockType::Grass;
                    blk.sunlight = 1.0f;
                    blk.lightColor = { 0, 0, 0 };
                    data[x][y][z] = blk;
                }
            }
        }
    }
}
