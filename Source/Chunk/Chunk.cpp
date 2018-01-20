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
    if(x < 0 || x >= CHUNK_SECTION_SIZE ||
       z < 0 || z >= CHUNK_SECTION_SIZE ||
       y < 0 || y >= CHUNK_MAX_HEIGHT)
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
    if(x < 0 || x >= CHUNK_SECTION_SIZE ||
       z < 0 || z >= CHUNK_SECTION_SIZE ||
       y < 0 || y >= CHUNK_MAX_HEIGHT)
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
    //std::cerr << "model changed (" << ckPos_.x << " " << section << " " << ckPos_.z << std::endl;
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

void Chunk::Render(ChunkSectionRenderQueue *renderQueue)
{
    assert(renderQueue != nullptr);

    ChunkSectionModels *models;
    for(int section = 0; section != CHUNK_SECTION_NUM; ++section)
    {
        if(models = GetModels(section))
        {
            //basic renderer
            for(int b = 0; b != BASIC_RENDERER_TEXTURE_NUM; ++b)
                renderQueue->basic[b].AddModel(&models->basic[b]);
        }
    }
}
