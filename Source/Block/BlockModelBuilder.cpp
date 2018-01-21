/*================================================================
Filename: BlockModelBuilder.cpp
Date: 2018.1.18
Created by AirGuanZ
================================================================*/
#include <cassert>

#include "../Renderer/BasicRenderer.h"
#include "BlockInfoManager.h"
#include "BlockModelBuilder.h"

const BlockModelBuilder *GetBlockModelBuilder(BlockType type)
{
    static const BlockModelBuilder_Null              builder_Null;
    static const BlockModelBuilder_BasicRenderer_Box builder_BasicRenderer_Null;
    static const BlockModelBuilder * const rt[2][2] =
    {
        { &builder_Null, &builder_Null },
        { &builder_Null, &builder_BasicRenderer_Null }
    };
    const BlockInfo &info = BlockInfoManager::GetInstance().GetBlockInfo(type);
    return rt[static_cast<std::underlying_type_t<BlockRenderer>>(info.renderer)]
             [static_cast<std::underlying_type_t<BlockShape>>(info.shape)];
}

void BlockModelBuilder_Null::Build(
    const Vector3 &posOffset,
    const Block &blk,
    const Block &pX, const Block &nX,
    const Block &pY, const Block &nY,
    const Block &pZ, const Block &nZ,
    ChunkSectionModels *models) const
{

}

void BlockModelBuilder_BasicRenderer_Box::Build(
    const Vector3 &posOffset,
    const Block &blk,
    const Block &pX, const Block &nX,
    const Block &pY, const Block &nY,
    const Block &pZ, const Block &nZ,
    ChunkSectionModels *models) const
{
    assert(models != nullptr);
    BlockInfoManager &infoMgr = BlockInfoManager::GetInstance();
    const BlockInfo &info = infoMgr.GetBlockInfo(blk.type);

    constexpr float TEX_GRID_SIZE = 1.0f / BASIC_RENDERER_TEXTURE_BLOCK_SIZE;
    Vector3 lightColor(blk.lightColor.r, blk.lightColor.g, blk.lightColor.b);
    BasicModel &model = models->basic[info.basicBoxTexPos[0]];

    auto AddFace = [&](const Vector3 &vtx0, const Vector3 &vtx1,
                       const Vector3 &vtx2, const Vector3 &vtx3,
                       int basicBoxTexPosIdx,
                       BasicModel &output)
    {
        float texBaseU = info.basicBoxTexPos[basicBoxTexPosIdx]
            % BASIC_RENDERER_TEXTURE_BLOCK_SIZE * TEX_GRID_SIZE;
        float texBaseV = info.basicBoxTexPos[basicBoxTexPosIdx]
            / BASIC_RENDERER_TEXTURE_BLOCK_SIZE * TEX_GRID_SIZE;
        UINT16 idxStart = static_cast<UINT16>(output.GetVerticesCount());

        output.AddVertex({ 
            posOffset + vtx0,
            { texBaseU + 0.0015f, texBaseV + TEX_GRID_SIZE - 0.0015f },
            lightColor, blk.sunlight
        });
        output.AddVertex({
            posOffset + vtx1,
            { texBaseU + 0.0015f, texBaseV + 0.0015f },
            lightColor, blk.sunlight
        });
        output.AddVertex({
            posOffset + vtx2,
            { texBaseU + TEX_GRID_SIZE - 0.0015f, texBaseV + 0.0015f },
            lightColor, blk.sunlight
        });
        output.AddVertex({
            posOffset + vtx3,
            { texBaseU + TEX_GRID_SIZE - 0.0015f, texBaseV + TEX_GRID_SIZE - 0.0015f },
            lightColor, blk.sunlight
        });

        output.AddIndex(idxStart);
        output.AddIndex(idxStart + 1);
        output.AddIndex(idxStart + 2);

        output.AddIndex(idxStart);
        output.AddIndex(idxStart + 2);
        output.AddIndex(idxStart + 3);
    };

    //x+
    if(infoMgr.IsFaceVisible(blk.type, pX.type))
    {
        AddFace({ 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f },
                { 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f },
                1, model);
    }
    //x-
    if(infoMgr.IsFaceVisible(blk.type, nX.type))
    {
        AddFace({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f },
                { 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f },
                2, model);
    }
    //y+
    if(infoMgr.IsFaceVisible(blk.type, pY.type))
    {
        AddFace({ 0.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f },
                { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f },
                3, model);
    }
    //y-
    if(infoMgr.IsFaceVisible(blk.type, nY.type))
    {
        AddFace({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f },
                { 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f },
                4, model);
    }
    //z+
    if(infoMgr.IsFaceVisible(blk.type, pZ.type))
    {
        AddFace({ 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f },
                { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 1.0f },
                5, model);
    }
    //z-
    if(infoMgr.IsFaceVisible(blk.type, nZ.type))
    {
        AddFace({ 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 0.0f },
                { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f },
                6, model);
    }
}
