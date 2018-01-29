/*================================================================
Filename: Chunk.cpp
Date: 2018.1.18
Created by AirGuanZ
================================================================*/
#include <cstring>

#include "../Block/BlockInfoManager.h"
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
            for(int b = 0; b != LIQUID_RENDERER_TEXTURE_NUM; ++b)
                renderQueue->liquid[b].AddModel(&models->liquid[b]);
        }
    }
}
