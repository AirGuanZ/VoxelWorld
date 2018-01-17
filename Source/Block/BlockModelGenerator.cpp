/*================================================================
Filename: BlockModelGenerator.cpp
Date: 2018.1.17
Created by AirGuanZ
================================================================*/
#include <cassert>

#include "BlockInfoManager.h"
#include "BlockModelGenerator.h"

const BlockModelGenerator *GetBlockModelGenerator(const Block &block)
{
    static const BlockModelGenerator_BasicRenderer_Box BMG_BasicRenderer_Box;
    static const BlockModelGenerator_Null              BMG_Null;

    const BlockInfo &info = BlockInfoManager::GetInstance().GetBlockInfo(block.type);
    switch(info.shape)
    {
    case BlockShape::Null:
        return &BMG_Null;
    case BlockShape::Box:
        switch(info.renderer)
        {
        case BlockRenderer::Null:
            return &BMG_Null;
        case BlockRenderer::BasicRenderer:
            return &BMG_BasicRenderer_Box;
        default:
            abort();
        }
    default:
        abort();
    }
    return nullptr;
}

void BlockModelGenerator_Null::AddTriangles(
    const Vector3 &posOffset,
    const Block &blk,
    const Block &pX, const Block &nX,
    const Block &pY, const Block &nY,
    const Block &pZ, const Block &nZ,
    ChunkSectionModels *models) const
{
    //do nothing
}

void BlockModelGenerator_BasicRenderer_Box::AddTriangles(
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
    assert(info.shape == BlockShape::Box && info.renderer == BlockRenderer::BasicRenderer);

    constexpr float TEX_GRID_SIZE_U = 1.0f / BASIC_RENDERER_TEXTURE_BLOCK_SIZE;
    constexpr float TEX_GRID_SIZE_V = 1.0f / BASIC_RENDERER_TEXTURE_BLOCK_SIZE;

    Vector3 lightColor = Vector3(blk.lightColor.r, blk.lightColor.g, blk.lightColor.b);

    //x+
    if(infoMgr.IsFaceVisible(blk.type, pX.type))
    {
        float texGridBaseV = info.basicBoxTexPos[1] / BASIC_RENDERER_TEXTURE_BLOCK_SIZE * TEX_GRID_SIZE_V;
        float texGridBaseU = info.basicBoxTexPos[1] % BASIC_RENDERER_TEXTURE_BLOCK_SIZE * TEX_GRID_SIZE_U;

        BasicRenderer::Vertex vertices[] =
        {
            { { 1.0f, 0.0f, 1.0f }, { texGridBaseU, texGridBaseV + TEX_GRID_SIZE_V },                   lightColor },
            { { 1.0f, 1.0f, 1.0f }, { texGridBaseU, texGridBaseV },                                     lightColor },
            { { 1.0f, 1.0f, 0.0f }, { texGridBaseU + TEX_GRID_SIZE_U, texGridBaseV },                   lightColor },
            { { 1.0f, 0.0f, 1.0f }, { texGridBaseU, texGridBaseV + TEX_GRID_SIZE_V },                   lightColor },
            { { 1.0f, 1.0f, 0.0f }, { texGridBaseU + TEX_GRID_SIZE_U, texGridBaseV },                   lightColor },
            { { 1.0f, 0.0f, 0.0f }, { texGridBaseU + TEX_GRID_SIZE_U, texGridBaseV + TEX_GRID_SIZE_V }, lightColor }
        };

        models->basicModel[info.basicBoxTexPos[0]].AddTriangle(vertices);
        models->basicModel[info.basicBoxTexPos[0]].AddTriangle(vertices + 3);
    }

    //x-
    if(infoMgr.IsFaceVisible(blk.type, nX.type))
    {
        float texGridBaseV = info.basicBoxTexPos[2] / BASIC_RENDERER_TEXTURE_BLOCK_SIZE * TEX_GRID_SIZE_V;
        float texGridBaseU = info.basicBoxTexPos[2] % BASIC_RENDERER_TEXTURE_BLOCK_SIZE * TEX_GRID_SIZE_U;

        BasicRenderer::Vertex vertices[] =
        {
            { { 0.0f, 0.0f, 0.0f },{ texGridBaseU, texGridBaseV + TEX_GRID_SIZE_V },                   lightColor },
            { { 0.0f, 1.0f, 0.0f },{ texGridBaseU, texGridBaseV },                                     lightColor },
            { { 0.0f, 1.0f, 1.0f },{ texGridBaseU + TEX_GRID_SIZE_U, texGridBaseV },                   lightColor },
            { { 0.0f, 0.0f, 0.0f },{ texGridBaseU, texGridBaseV + TEX_GRID_SIZE_V },                   lightColor },
            { { 0.0f, 1.0f, 1.0f },{ texGridBaseU + TEX_GRID_SIZE_U, texGridBaseV },                   lightColor },
            { { 0.0f, 0.0f, 1.0f },{ texGridBaseU + TEX_GRID_SIZE_U, texGridBaseV + TEX_GRID_SIZE_V }, lightColor }
        };

        models->basicModel[info.basicBoxTexPos[0]].AddTriangle(vertices);
        models->basicModel[info.basicBoxTexPos[0]].AddTriangle(vertices + 3);
    }

    //y+
    if(infoMgr.IsFaceVisible(blk.type, pY.type))
    {
        float texGridBaseV = info.basicBoxTexPos[3] / BASIC_RENDERER_TEXTURE_BLOCK_SIZE * TEX_GRID_SIZE_V;
        float texGridBaseU = info.basicBoxTexPos[3] % BASIC_RENDERER_TEXTURE_BLOCK_SIZE * TEX_GRID_SIZE_U;

        BasicRenderer::Vertex vertices[] =
        {
            { { 0.0f, 1.0f, 1.0f },{ texGridBaseU, texGridBaseV + TEX_GRID_SIZE_V },                   lightColor },
            { { 0.0f, 1.0f, 0.0f },{ texGridBaseU, texGridBaseV },                                     lightColor },
            { { 1.0f, 1.0f, 0.0f },{ texGridBaseU + TEX_GRID_SIZE_U, texGridBaseV },                   lightColor },
            { { 0.0f, 1.0f, 1.0f },{ texGridBaseU, texGridBaseV + TEX_GRID_SIZE_V },                   lightColor },
            { { 1.0f, 1.0f, 0.0f },{ texGridBaseU + TEX_GRID_SIZE_U, texGridBaseV },                   lightColor },
            { { 1.0f, 1.0f, 1.0f },{ texGridBaseU + TEX_GRID_SIZE_U, texGridBaseV + TEX_GRID_SIZE_V }, lightColor }
        };

        models->basicModel[info.basicBoxTexPos[0]].AddTriangle(vertices);
        models->basicModel[info.basicBoxTexPos[0]].AddTriangle(vertices + 3);
    }

    //y-
    if(infoMgr.IsFaceVisible(blk.type, nY.type))
    {
        float texGridBaseV = info.basicBoxTexPos[4] / BASIC_RENDERER_TEXTURE_BLOCK_SIZE * TEX_GRID_SIZE_V;
        float texGridBaseU = info.basicBoxTexPos[4] % BASIC_RENDERER_TEXTURE_BLOCK_SIZE * TEX_GRID_SIZE_U;

        BasicRenderer::Vertex vertices[] =
        {
            { { 1.0f, 0.0f, 0.0f },{ texGridBaseU, texGridBaseV + TEX_GRID_SIZE_V },                   lightColor },
            { { 1.0f, 0.0f, 1.0f },{ texGridBaseU, texGridBaseV },                                     lightColor },
            { { 0.0f, 0.0f, 1.0f },{ texGridBaseU + TEX_GRID_SIZE_U, texGridBaseV },                   lightColor },
            { { 1.0f, 0.0f, 0.0f },{ texGridBaseU, texGridBaseV + TEX_GRID_SIZE_V },                   lightColor },
            { { 0.0f, 0.0f, 1.0f },{ texGridBaseU + TEX_GRID_SIZE_U, texGridBaseV },                   lightColor },
            { { 0.0f, 0.0f, 0.0f },{ texGridBaseU + TEX_GRID_SIZE_U, texGridBaseV + TEX_GRID_SIZE_V }, lightColor }
        };

        models->basicModel[info.basicBoxTexPos[0]].AddTriangle(vertices);
        models->basicModel[info.basicBoxTexPos[0]].AddTriangle(vertices + 3);
    }

    //z+
    if(infoMgr.IsFaceVisible(blk.type, pZ.type))
    {
        float texGridBaseV = info.basicBoxTexPos[5] / BASIC_RENDERER_TEXTURE_BLOCK_SIZE * TEX_GRID_SIZE_V;
        float texGridBaseU = info.basicBoxTexPos[5] % BASIC_RENDERER_TEXTURE_BLOCK_SIZE * TEX_GRID_SIZE_U;

        BasicRenderer::Vertex vertices[] =
        {
            { { 0.0f, 0.0f, 1.0f },{ texGridBaseU, texGridBaseV + TEX_GRID_SIZE_V },                   lightColor },
            { { 0.0f, 1.0f, 1.0f },{ texGridBaseU, texGridBaseV },                                     lightColor },
            { { 1.0f, 1.0f, 1.0f },{ texGridBaseU + TEX_GRID_SIZE_U, texGridBaseV },                   lightColor },
            { { 0.0f, 0.0f, 1.0f },{ texGridBaseU, texGridBaseV + TEX_GRID_SIZE_V },                   lightColor },
            { { 1.0f, 1.0f, 1.0f },{ texGridBaseU + TEX_GRID_SIZE_U, texGridBaseV },                   lightColor },
            { { 1.0f, 0.0f, 1.0f },{ texGridBaseU + TEX_GRID_SIZE_U, texGridBaseV + TEX_GRID_SIZE_V }, lightColor }
        };

        models->basicModel[info.basicBoxTexPos[0]].AddTriangle(vertices);
        models->basicModel[info.basicBoxTexPos[0]].AddTriangle(vertices + 3);
    }

    //z-
    if(infoMgr.IsFaceVisible(blk.type, nZ.type))
    {
        float texGridBaseV = info.basicBoxTexPos[6] / BASIC_RENDERER_TEXTURE_BLOCK_SIZE * TEX_GRID_SIZE_V;
        float texGridBaseU = info.basicBoxTexPos[6] % BASIC_RENDERER_TEXTURE_BLOCK_SIZE * TEX_GRID_SIZE_U;

        BasicRenderer::Vertex vertices[] =
        {
            { { 1.0f, 0.0f, 0.0f },{ texGridBaseU, texGridBaseV + TEX_GRID_SIZE_V },                   lightColor },
            { { 1.0f, 1.0f, 0.0f },{ texGridBaseU, texGridBaseV },                                     lightColor },
            { { 0.0f, 1.0f, 0.0f },{ texGridBaseU + TEX_GRID_SIZE_U, texGridBaseV },                   lightColor },
            { { 1.0f, 0.0f, 0.0f },{ texGridBaseU, texGridBaseV + TEX_GRID_SIZE_V },                   lightColor },
            { { 0.0f, 1.0f, 0.0f },{ texGridBaseU + TEX_GRID_SIZE_U, texGridBaseV },                   lightColor },
            { { 0.0f, 0.0f, 0.0f },{ texGridBaseU + TEX_GRID_SIZE_U, texGridBaseV + TEX_GRID_SIZE_V }, lightColor }
        };

        models->basicModel[info.basicBoxTexPos[0]].AddTriangle(vertices);
        models->basicModel[info.basicBoxTexPos[0]].AddTriangle(vertices + 3);
    }
}
