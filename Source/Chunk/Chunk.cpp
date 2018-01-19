/*================================================================
Filename: Chunk.cpp
Date: 2018.1.18
Created by AirGuanZ
================================================================*/
#include <cstring>

#include "../Utility/HelperFunctions.h"
#include "Chunk.h"
#include "ChunkManager.h"

Chunk::Chunk(ChunkManager *ckMgr, const IntVectorXZ &ckPos)
    : ckMgr_(ckMgr), ckPos_(ckPos)
{
    assert(ckMgr != nullptr);
    std::memset(models_, 0, sizeof(models_));
}

Chunk::~Chunk(void)
{
    for(ChunkSectionModels *model : models_)
        Helper::SafeDeleteObjects(model);
}

IntVectorXZ Chunk::GetPosition(void) const
{
    return ckPos_;
}

Block &Chunk::GetBlock(int x, int y, int z)
{
    if(0 <= x && x < CHUNK_SECTION_SIZE &&
       0 <= z && z < CHUNK_SECTION_SIZE &&
       0 <= y && y < CHUNK_MAX_HEIGHT)
    {
        return ckMgr_->GetBlock(
            ChunkXZ_To_BlockXZ(ckPos_.x) + x,
            y,
            ChunkXZ_To_BlockXZ(ckPos_.z) + z);
    }
    return blocks_[x][y][z];
}

void Chunk::SetBlock(int x, int y, int z, const Block &blk)
{
    if(0 <= x && x < CHUNK_SECTION_SIZE &&
       0 <= z && z < CHUNK_SECTION_SIZE &&
       0 <= y && y < CHUNK_MAX_HEIGHT)
    {
        ckMgr_->SetBlock(
            ChunkXZ_To_BlockXZ(ckPos_.x) + x,
            y,
            ChunkXZ_To_BlockXZ(ckPos_.z) + z,
            blk);
    }
    blocks_[x][y][z] = blk;
}

void Chunk::SetModel(int section, ChunkSectionModels *model)
{
    assert(0 <= section && section < CHUNK_SECTION_NUM);
    Helper::SafeDeleteObjects(models_[section]);
    models_[section] = model;
}

Chunk::BlockData &Chunk::GetBlockData(void)
{
    return blocks_;
}

ChunkSectionModels *Chunk::GetModels(int section)
{
    assert(0 <= section && section < CHUNK_SECTION_NUM);
    return models_[section];
}
