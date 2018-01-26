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

namespace
{

}

void OakGenerator_V0::Make(Chunk *ck, std::vector<IntVector3> &lightUpdates) const
{
    assert(ck != nullptr);

    Chunk::BlockData &blks = ck->GetBlockData();
    int h = ck->GetHeightMap()[5][5];
    if(blks[5][h][5].type != BlockType::GrassBox)
        --h;

    for(int y = h + 1; y != h + 6; ++y)
    {
        blks[5][y][5].type = BlockType::Wood;
        SetLight(blks[5][y][5], 0, 0, 0, LIGHT_COMPONENT_MAX + 1);
        lightUpdates.push_back({ ChunkXZ_To_BlockXZ(ck->GetPosition().x) + 5, y,
                                 ChunkXZ_To_BlockXZ(ck->GetPosition().z) + 5 });
    }

    for(int x = 4; x <= 6; ++x)
    {
        for(int z = 4; z <= 6; ++z)
        {
            blks[x][h + 6][z].type = BlockType::Leaf;
            SetLight(blks[5][h + 6][5], 0, 0, 0, LIGHT_COMPONENT_MAX + 1);
            lightUpdates.push_back({ ChunkXZ_To_BlockXZ(ck->GetPosition().x) + x, h + 6,
                                     ChunkXZ_To_BlockXZ(ck->GetPosition().z) + z });
            ck->GetHeightMap()[x][z] = h + 6;
        }
    }
}
