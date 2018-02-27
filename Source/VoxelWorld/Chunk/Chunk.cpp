/*================================================================
Filename: Chunk.cpp
Date: 2018.1.18
Created by AirGuanZ
================================================================*/
#include <cstring>

#include <Utility/HelperFunctions.h>

#include <Block/BlockInfoManager.h>
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

void Chunk::Render(const Camera &cam, ChunkSectionRenderQueue *renderQueue)
{
    assert(renderQueue != nullptr);

    //ChunkµÄAABBºÐ
    float xL = static_cast<float>(ChunkXZ_To_BlockXZ(ckPos_.x));
    float zL = static_cast<float>(ChunkXZ_To_BlockXZ(ckPos_.z));
    float xH = xL + CHUNK_SECTION_SIZE, zH = zL + CHUNK_SECTION_SIZE;
    float yL = 0.0f;

    for(int section = 0; section != CHUNK_SECTION_NUM; ++section)
    {
        ChunkSectionModels *models;
        if((models = GetModels(section)) &&
            cam.InFrustum(AABB({ xL, yL, zL }, { xH, yL + CHUNK_SECTION_SIZE, zH })))
        {
            for(int b = 0; b != BASIC_RENDERER_TEXTURE_NUM; ++b)
                renderQueue->basic[b].AddModel(&models->basic[b]);
            for(int b = 0; b != CARVE_RENDERER_TEXTURE_NUM; ++b)
                renderQueue->carve[b].AddModel(&models->carve[b]);
            for(int b = 0; b != LIQUID_RENDERER_TEXTURE_NUM; ++b)
                renderQueue->liquid[b].AddModel(&models->liquid[b]);
        }
        yL += static_cast<float>(CHUNK_SECTION_SIZE);
    }
}
