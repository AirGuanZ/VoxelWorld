/*================================================================
Filename: ChunkModelBuilder.cpp
Date: 2018.1.18
Created by AirGuanZ
================================================================*/
#include "../Block/BlockModelBuilder.h"
#include "ChunkModelBuilder.h"

ChunkModelBuilder::ChunkModelBuilder(ChunkManager *ckMgr, Chunk *ck, int section)
    : ckMgr_(ckMgr), ck_(ck), section_(section)
{

}

ChunkSectionModels *ChunkModelBuilder::Build(void)
{
    assert(ckMgr_ != nullptr && ck_ != nullptr);

    int yBase = ChunkSectionIndex_To_BlockY(section_);
    ChunkSectionModels *models = new ChunkSectionModels;
    for(int x = 0; x != CHUNK_SECTION_SIZE; ++x)
    {
        for(int y = yBase; y != yBase + CHUNK_SECTION_SIZE; ++y)
        {
            for(int z = 0; z != CHUNK_SECTION_SIZE; ++z)
            {
                const Block &pX = ck_->GetBlock(x + 1, y, z),
                            &nX = ck_->GetBlock(x - 1, y, z),
                            &pZ = ck_->GetBlock(x, y, z + 1),
                            &nZ = ck_->GetBlock(x, y, z - 1),
                            &pY = ck_->GetBlock(x, y + 1, z),
                            &nY = ck_->GetBlock(x, y - 1, z);
                GetBlockModelBuilder(ck_->GetBlock(x, y, z).type)->Build(
                    Vector3(x, y - yBase, z), ck_->GetBlock(x, y, z),
                    pX, nX, pY, nY, pZ, nZ, models);
            }
        }
    }

    return models;
}
