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

inline bool OutOfChunk(std::int32_t x, std::int32_t y, std::int32_t z)
{
    return (x | (CHUNK_SECTION_SIZE - 1 - x) |
            y | (CHUNK_MAX_HEIGHT - 1 - y) |
            z | (CHUNK_SECTION_SIZE - 1 - z)) < 0;
}

Block &Chunk::GetBlock(int x, int y, int z)
{
    if(OutOfChunk(x, y, z))
    {
        return ckMgr_->GetBlock(
            ChunkXZ_To_BlockXZ(ckPos_.x) + x,
            y,
            ChunkXZ_To_BlockXZ(ckPos_.z) + z);
    }
    return blocks_[x][y][z];
}

const Block &Chunk::GetInternalBlock(int x, int y, int z) const
{
    if(OutOfChunk(x, y, z))
    {
        static const Block dummyBlock;
        return dummyBlock;
    }
    return blocks_[x][y][z];
}

void Chunk::SetBlock(int x, int y, int z, const Block &blk)
{
    int wX = ChunkXZ_To_BlockXZ(ckPos_.x) + x;
    int wZ = ChunkXZ_To_BlockXZ(ckPos_.z) + z;

    if(OutOfChunk(x, y, z))
    {
        ckMgr_->SetBlock(wX, y, wZ, blk);
        return;
    }

    blocks_[x][y][z] = blk;
    SetLight(blocks_[x][y][z], LIGHT_COMPONENT_MAX + 1, LIGHT_COMPONENT_MAX + 1,
                               LIGHT_COMPONENT_MAX + 1, LIGHT_COMPONENT_MAX + 1);
    if(y >= heightMap_[x][z])
    {
        int newH = CHUNK_MAX_HEIGHT - 1;
        while(newH > 0 && blocks_[x][newH][z].type == BlockType::Air)
            --newH;
        if(newH != heightMap_[x][z])
        {
            int L, H; std::tie(L, H) = std::minmax(newH, heightMap_[x][z]);
            while(H >= L)
            {
                ckMgr_->AddLightUpdate(wX, H, wZ);
                --H;
            }
            heightMap_[x][z] = newH;
        }
    }

    ckMgr_->AddLightUpdate(wX, y, wZ);
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

Chunk::HeightMap &Chunk::GetHeightMap(void)
{
    return heightMap_;
}

ChunkSectionModels *Chunk::GetModels(int section)
{
    assert(0 <= section && section < CHUNK_SECTION_NUM);
    return models_[section];
}

void Chunk::Render(ChunkSectionRenderQueue *renderQueue)
{
    assert(renderQueue != nullptr);

    for(int section = 0; section != CHUNK_SECTION_NUM; ++section)
    {
        ChunkSectionModels *models;
        if(models = GetModels(section))
        {
            for(int b = 0; b != BASIC_RENDERER_TEXTURE_NUM; ++b)
                renderQueue->basic[b].AddModel(&models->basic[b]);
            for(int b = 0; b != CARVE_RENDERER_TEXTURE_NUM; ++b)
                renderQueue->carve[b].AddModel(&models->carve[b]);
        }
    }
}
