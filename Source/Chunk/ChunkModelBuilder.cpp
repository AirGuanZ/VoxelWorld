/*================================================================
Filename: ChunkModelBuilder.cpp
Date: 2018.1.18
Created by AirGuanZ
================================================================*/
#include "../Block/BlockInfoManager.h"
#include "../Block/BlockModelBuilder.h"
#include "Chunk.h"
#include "ChunkManager.h"
#include "ChunkModelBuilder.h"
#include "ChunkTraversal.h"

ChunkModelBuilder::ChunkModelBuilder(ChunkManager *ckMgr, Chunk *ck, int section)
    : ckMgr_(ckMgr), ck_(ck), section_(section)
{

}

ChunkSectionModels *ChunkModelBuilder::Build(void)
{
    assert(ckMgr_ != nullptr && ck_ != nullptr);

    int xBase = ck_->GetXPosBase();
    int zBase = ck_->GetZPosBase();
    int yBase = ChunkSectionIndex_To_BlockY(section_);

    ChunkSectionModels *models = new ChunkSectionModels;

    for(const IntVector3 &f : ChunkTraversal::GetInstance().ChunkFaces())
    {
        int x = f.x + xBase, y = f.y + yBase, z = f.z + zBase;

        Block blk = ckMgr_->GetBlock(x, y, z);
        if(!BlockInfoManager::GetInstance().IsRenderable(blk.type))
            continue;

        Block pX = ckMgr_->GetBlock(x + 1, y, z),
              nX = ckMgr_->GetBlock(x - 1, y, z),
              pZ = ckMgr_->GetBlock(x, y, z + 1),
              nZ = ckMgr_->GetBlock(x, y, z - 1),
              pY = ckMgr_->GetBlock(x, y + 1, z),
              nY = ckMgr_->GetBlock(x, y - 1, z);
        const Block blks[3][3][3] =
        {
            {
                { ckMgr_->GetBlock(x - 1, y - 1, z - 1), ckMgr_->GetBlock(x - 1, y - 1, z), ckMgr_->GetBlock(x - 1, y - 1, z + 1) }, //[0][0]
                { ckMgr_->GetBlock(x - 1, y, z - 1),     ckMgr_->GetBlock(x - 1, y, z),     ckMgr_->GetBlock(x - 1, y, z + 1) },     //[0][1]
                { ckMgr_->GetBlock(x - 1, y + 1, z - 1), ckMgr_->GetBlock(x - 1, y + 1, z), ckMgr_->GetBlock(x - 1, y + 1, z + 1) }, //[0][2]
            },
            {
                { ckMgr_->GetBlock(x, y - 1, z - 1), ckMgr_->GetBlock(x, y - 1, z), ckMgr_->GetBlock(x, y - 1, z + 1) },             //[0][0]
                { ckMgr_->GetBlock(x, y, z - 1),     ckMgr_->GetBlock(x, y, z),     ckMgr_->GetBlock(x, y, z + 1) },                 //[0][1]
                { ckMgr_->GetBlock(x, y + 1, z - 1), ckMgr_->GetBlock(x, y + 1, z), ckMgr_->GetBlock(x, y + 1, z + 1) },             //[0][2]
            },
            {
                { ckMgr_->GetBlock(x + 1, y - 1, z - 1), ckMgr_->GetBlock(x + 1, y - 1, z), ckMgr_->GetBlock(x + 1, y - 1, z + 1) }, //[0][0]
                { ckMgr_->GetBlock(x + 1, y, z - 1),     ckMgr_->GetBlock(x + 1, y, z),     ckMgr_->GetBlock(x + 1, y, z + 1) },     //[0][1]
                { ckMgr_->GetBlock(x + 1, y + 1, z - 1), ckMgr_->GetBlock(x + 1, y + 1, z), ckMgr_->GetBlock(x + 1, y + 1, z + 1) }, //[0][2]
            }
        };
        GetBlockModelBuilder(blk.type)->Build(
            Vector3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)),
            blks, models);
    }

    for(int Lx = 1; Lx < CHUNK_SECTION_SIZE - 1; ++Lx)
    {
        int x = Lx + xBase;
        for(int Lz = 1; Lz < CHUNK_SECTION_SIZE - 1; ++Lz)
        {
            int z = Lz + zBase;
            for(int Ly = 1; Ly < CHUNK_SECTION_SIZE - 1; ++Ly)
            {
                int y = Ly + yBase;

                Block blk = ck_->GetBlock(Lx, y, Lz);
                if(!BlockInfoManager::GetInstance().IsRenderable(blk.type))
                    continue;

                const Block blks[3][3][3] =
                {
                    {
                        { ck_->GetBlock(Lx - 1, y - 1, Lz - 1), ck_->GetBlock(Lx - 1, y - 1, Lz), ck_->GetBlock(Lx - 1, y - 1, Lz + 1) }, //[0][0]
                        { ck_->GetBlock(Lx - 1, y, Lz - 1),     ck_->GetBlock(Lx - 1, y, Lz),     ck_->GetBlock(Lx - 1, y, Lz + 1) },     //[0][1]
                        { ck_->GetBlock(Lx - 1, y + 1, Lz - 1), ck_->GetBlock(Lx - 1, y + 1, Lz), ck_->GetBlock(Lx - 1, y + 1, Lz + 1) }, //[0][2]
                    },
                    {
                        { ck_->GetBlock(Lx, y - 1, Lz - 1), ck_->GetBlock(Lx, y - 1, Lz), ck_->GetBlock(Lx, y - 1, Lz + 1) },             //[0][0]
                        { ck_->GetBlock(Lx, y, Lz - 1),     ck_->GetBlock(Lx, y, Lz),     ck_->GetBlock(Lx, y, Lz + 1) },                 //[0][1]
                        { ck_->GetBlock(Lx, y + 1, Lz - 1), ck_->GetBlock(Lx, y + 1, Lz), ck_->GetBlock(Lx, y + 1, Lz + 1) },             //[0][2]
                    },
                    {
                        { ck_->GetBlock(Lx + 1, y - 1, Lz - 1), ck_->GetBlock(Lx + 1, y - 1, Lz), ck_->GetBlock(Lx + 1, y - 1, Lz + 1) }, //[0][0]
                        { ck_->GetBlock(Lx + 1, y, Lz - 1),     ck_->GetBlock(Lx + 1, y, Lz),     ck_->GetBlock(Lx + 1, y, Lz + 1) },     //[0][1]
                        { ck_->GetBlock(Lx + 1, y + 1, Lz - 1), ck_->GetBlock(Lx + 1, y + 1, Lz), ck_->GetBlock(Lx + 1, y + 1, Lz + 1) }, //[0][2]
                    }
                };
                GetBlockModelBuilder(blk.type)->Build(
                    Vector3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)),
                    blks, models);
            }
        }
    }

    for(int i = 0; i != BASIC_RENDERER_TEXTURE_NUM; ++i)
        models->basic[i].MakeVertexBuffer();
    for(int i = 0; i != CARVE_RENDERER_TEXTURE_NUM; ++i)
        models->carve[i].MakeVertexBuffer();
    for(int i = 0; i != LIQUID_RENDERER_TEXTURE_NUM; ++i)
        models->liquid[i].MakeVertexBuffer();
    return models;
}
