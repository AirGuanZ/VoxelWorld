/*================================================================
Filename: V3/LandGenerator.cpp
Date: 2018.4.16
Created by AirGuanZ
================================================================*/
#include <cassert>

#include <World\Land\V3\LandGenerator_V3.h>

void ::LandGenerator_V3::LandGenerator::GenerateLand(Chunk *ck)
{
    assert(ck != nullptr);

    for(int x = 0; x != CHUNK_SECTION_SIZE; ++x)
    {
        for(int z = 0; z != CHUNK_SECTION_SIZE; ++z)
        {
            for(int y = 0; y != 10; ++y)
                ck->SetBlockType(x, y, z, BlockType::Dirt);
            ck->SetBlockType(x, 10, z, BlockType::GrassBox);
            for(int y = 11; y != CHUNK_MAX_HEIGHT; ++y)
                ck->SetBlockType(x, y, z, BlockType::Air);

            ck->SetHeight(x, z, 10);

            for(int y = 0; y <= 10; ++y)
                ck->SetBlockLight(x, y, z, LIGHT_ALL_MIN);
            for(int y = 11; y != CHUNK_MAX_HEIGHT; ++y)
                ck->SetBlockLight(x, y, z, LIGHT_MIN_MIN_MIN_MAX);
        }
    }
}
