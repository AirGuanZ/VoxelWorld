/*================================================================
Filename: BlockModelBuilder.cpp
Date: 2018.1.18
Created by AirGuanZ
================================================================*/
#include <cassert>

#include "../Renderer/BasicRenderer.h"
#include "../Renderer/CarveRenderer.h"
#include "BlockInfoManager.h"
#include "BlockModelBuilder.h"

const BlockModelBuilder *GetBlockModelBuilder(BlockType type)
{
    static const BlockModelBuilder_Null                builder_Null;
    static const BlockModelBuilder_BasicRenderer_Box   builder_BasicRenderer_Box;
    static const BlockModelBuilder_CarveRenderer_Box   builder_CarveRenderer_Box;
    static const BlockModelBuilder_CarveRenderer_Cross builder_CarveRenderer_Cross;
    static const BlockModelBuilder * const rt[3][3] =
    {
        { &builder_Null, &builder_Null,              &builder_Null },
        { &builder_Null, &builder_BasicRenderer_Box, &builder_Null },
        { &builder_Null, &builder_CarveRenderer_Box, &builder_CarveRenderer_Cross }
    };
    const BlockInfo &info = BlockInfoManager::GetInstance().GetBlockInfo(type);
    return rt[static_cast<std::underlying_type_t<BlockRenderer>>(info.renderer)]
             [static_cast<std::underlying_type_t<BlockShape>>(info.shape)];
}

void BlockModelBuilder_Null::Build(
    const Vector3 &posOffset,
    const Block(&blks)[3][3][3],
    ChunkSectionModels *models) const
{

}

inline Color BlockAO(BlockLight l0, BlockLight l1, BlockLight l2, BlockLight l3)
{
    return Color(0.15f, 0.15f, 0.15f, 0.15f) + 0.85f * 0.25f * (LightToRGBA(l0) + LightToRGBA(l1) +
                                    LightToRGBA(l2) + LightToRGBA(l3));
}

void BlockModelBuilder_BasicRenderer_Box::Build(
    const Vector3 &posOffset,
    const Block(&blks)[3][3][3],
    ChunkSectionModels *models) const
{
    assert(models != nullptr);

    const Block &blk = blks[1][1][1],
                &pX  = blks[2][1][1],
                &nX  = blks[0][1][1],
                &pY  = blks[1][2][1],
                &nY  = blks[1][0][1],
                &pZ  = blks[1][1][2],
                &nZ  = blks[1][1][0];

    BlockInfoManager &infoMgr = BlockInfoManager::GetInstance();
    const BlockInfo &info = infoMgr.GetBlockInfo(blk.type);

    constexpr float TEX_GRID_SIZE = 1.0f / BASIC_RENDERER_TEXTURE_BLOCK_SIZE;
    BasicModel &model = models->basic[info.basicBoxTexPos[0]];

    auto AddFace = [&](const Vector3 &vtx0, const Vector3 &vtx1,
                       const Vector3 &vtx2, const Vector3 &vtx3,
                       const Color &c0, const Color &c1,
                       const Color &c2, const Color &c3,
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
            { c0.R(), c0.G(), c0.B() }, c0.A()
        });
        output.AddVertex({
            posOffset + vtx1,
            { texBaseU + 0.0015f, texBaseV + 0.0015f },
            { c1.R(), c1.G(), c1.B() }, c1.A()
        });
        output.AddVertex({
            posOffset + vtx2,
            { texBaseU + TEX_GRID_SIZE - 0.0015f, texBaseV + 0.0015f },
            { c2.R(), c2.G(), c2.B() }, c2.A()
        });
        output.AddVertex({
            posOffset + vtx3,
            { texBaseU + TEX_GRID_SIZE - 0.0015f, texBaseV + TEX_GRID_SIZE - 0.0015f },
            { c3.R(), c3.G(), c3.B() }, c3.A()
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
                BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO * BlockAO(
					blks[2][0][1].rgbs, blks[2][1][1].rgbs,
                    blks[2][1][2].rgbs, blks[2][0][2].rgbs),
                BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO * BlockAO(
					blks[2][1][1].rgbs, blks[2][2][1].rgbs,
                    blks[2][2][2].rgbs, blks[2][1][2].rgbs),
                BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO * BlockAO(
					blks[2][1][0].rgbs, blks[2][2][0].rgbs,
                    blks[2][2][1].rgbs, blks[2][1][1].rgbs),
                BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO * BlockAO(
					blks[2][0][0].rgbs, blks[2][1][0].rgbs,
                    blks[2][1][1].rgbs, blks[2][0][1].rgbs),
                1, model);
    }
    //x-
    if(infoMgr.IsFaceVisible(blk.type, nX.type))
    {
        AddFace({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f },
                { 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f },
                BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO * BlockAO(
					blks[0][0][0].rgbs, blks[0][1][0].rgbs,
                    blks[0][1][1].rgbs, blks[0][0][1].rgbs),
                BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO * BlockAO(
					blks[0][1][0].rgbs, blks[0][2][0].rgbs,
                    blks[0][2][1].rgbs, blks[0][1][1].rgbs),
                BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO * BlockAO(
					blks[0][1][1].rgbs, blks[0][2][1].rgbs,
                    blks[0][2][2].rgbs, blks[0][1][2].rgbs),
                BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO * BlockAO(
					blks[0][0][1].rgbs, blks[0][1][1].rgbs,
                    blks[0][1][2].rgbs, blks[0][0][2].rgbs),
                2, model);
    }
    //y+
    if(infoMgr.IsFaceVisible(blk.type, pY.type))
    {
        AddFace({ 0.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f },
                { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f },
                BlockAO(
					blks[0][2][2].rgbs, blks[0][2][1].rgbs,
                    blks[1][2][1].rgbs, blks[1][2][2].rgbs),
                BlockAO(
					blks[0][2][1].rgbs, blks[0][2][0].rgbs,
                    blks[1][2][0].rgbs, blks[1][2][1].rgbs),
                BlockAO(
					blks[1][2][1].rgbs, blks[1][2][0].rgbs,
                    blks[2][2][0].rgbs, blks[2][2][1].rgbs),
                BlockAO(
					blks[1][2][2].rgbs, blks[1][2][1].rgbs,
                    blks[2][2][1].rgbs, blks[2][2][2].rgbs),
                3, model);
    }
    //y-
    if(infoMgr.IsFaceVisible(blk.type, nY.type))
    {
        AddFace({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f },
                { 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f },
                BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO * BlockAO(
					blks[0][0][1].rgbs, blks[0][0][0].rgbs,
                    blks[1][0][0].rgbs, blks[1][0][1].rgbs),
                BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO * BlockAO(
					blks[0][0][2].rgbs, blks[0][0][1].rgbs,
                    blks[1][0][1].rgbs, blks[1][0][2].rgbs),
                BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO * BlockAO(
					blks[1][0][2].rgbs, blks[1][0][1].rgbs,
                    blks[2][0][1].rgbs, blks[2][0][2].rgbs),
                BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO * BlockAO(
					blks[1][0][1].rgbs, blks[1][0][0].rgbs,
                    blks[2][0][0].rgbs, blks[2][0][1].rgbs),
                4, model);
    }
    //z+
    if(infoMgr.IsFaceVisible(blk.type, pZ.type))
    {
        AddFace({ 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f },
                { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 1.0f },
                BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO * BlockAO(
					blks[0][0][2].rgbs, blks[0][1][2].rgbs,
                    blks[1][1][2].rgbs, blks[1][0][2].rgbs),
                BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO * BlockAO(
					blks[0][1][2].rgbs, blks[0][2][2].rgbs,
                    blks[1][2][2].rgbs, blks[1][1][2].rgbs),
                BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO * BlockAO(
					blks[1][1][2].rgbs, blks[1][2][2].rgbs,
                    blks[2][2][2].rgbs, blks[2][1][2].rgbs),
                BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO * BlockAO(
					blks[1][0][2].rgbs, blks[1][1][2].rgbs,
                    blks[2][1][2].rgbs, blks[2][0][2].rgbs),
                5, model);
    }
    //z-
    if(infoMgr.IsFaceVisible(blk.type, nZ.type))
    {
        AddFace({ 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 0.0f },
                { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f },
                BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO * BlockAO(
					blks[1][0][0].rgbs, blks[1][1][0].rgbs,
                    blks[2][1][0].rgbs, blks[2][0][0].rgbs),
                BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO * BlockAO(
					blks[1][1][0].rgbs, blks[1][2][0].rgbs,
                    blks[2][2][0].rgbs, blks[2][1][0].rgbs),
                BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO * BlockAO(
					blks[0][1][0].rgbs, blks[0][2][0].rgbs,
                    blks[1][2][0].rgbs, blks[1][1][0].rgbs),
                BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO * BlockAO(
					blks[0][0][0].rgbs, blks[0][1][0].rgbs,
                    blks[1][1][0].rgbs, blks[1][0][0].rgbs),
                6, model);
    }
}

void BlockModelBuilder_CarveRenderer_Box::Build(
    const Vector3 &posOffset,
    const Block(&blks)[3][3][3],
    ChunkSectionModels *models) const
{
    assert(models != nullptr);

    const Block &blk = blks[1][1][1],
                &pX  = blks[2][1][1],
                &nX  = blks[0][1][1],
                &pY  = blks[1][2][1],
                &nY  = blks[1][0][1],
                &pZ  = blks[1][1][2],
                &nZ  = blks[1][1][0];

    BlockInfoManager &infoMgr = BlockInfoManager::GetInstance();
    const BlockInfo &info = infoMgr.GetBlockInfo(blk.type);

    constexpr float TEX_GRID_SIZE = 1.0f / CARVE_RENDERER_TEXTURE_BLOCK_SIZE;
    CarveModel &model = models->carve[info.carveBoxTexPos[0]];

    auto AddFace = [&](const Vector3 &vtx0, const Vector3 &vtx1,
                       const Vector3 &vtx2, const Vector3 &vtx3,
                       const Color &c0, const Color &c1,
                       const Color &c2, const Color &c3,
                       int carveBoxTexPosIdx,
                       CarveModel &output)
    {
        float texBaseU = info.carveBoxTexPos[carveBoxTexPosIdx]
            % CARVE_RENDERER_TEXTURE_BLOCK_SIZE * TEX_GRID_SIZE;
        float texBaseV = info.carveBoxTexPos[carveBoxTexPosIdx]
            / CARVE_RENDERER_TEXTURE_BLOCK_SIZE * TEX_GRID_SIZE;
        UINT16 idxStart = static_cast<UINT16>(output.GetVerticesCount());

        output.AddVertex({
            posOffset + vtx0,
            { texBaseU + 0.0015f, texBaseV + TEX_GRID_SIZE - 0.0015f },
            { c0.R(), c0.G(), c0.B() }, c0.A()
        });
        output.AddVertex({
            posOffset + vtx1,
            { texBaseU + 0.0015f, texBaseV + 0.0015f },
            { c1.R(), c1.G(), c1.B() }, c1.A()
        });
        output.AddVertex({
            posOffset + vtx2,
            { texBaseU + TEX_GRID_SIZE - 0.0015f, texBaseV + 0.0015f },
            { c2.R(), c2.G(), c2.B() }, c2.A()
        });
        output.AddVertex({
            posOffset + vtx3,
            { texBaseU + TEX_GRID_SIZE - 0.0015f, texBaseV + TEX_GRID_SIZE - 0.0015f },
            { c3.R(), c3.G(), c3.B() }, c3.A()
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
                BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO * BlockAO(
                    blks[2][0][1].rgbs, blks[2][1][1].rgbs,
                    blks[2][1][2].rgbs, blks[2][0][2].rgbs),
                BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO * BlockAO(
                    blks[2][1][1].rgbs, blks[2][2][1].rgbs,
                    blks[2][2][2].rgbs, blks[2][1][2].rgbs),
                BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO * BlockAO(
                    blks[2][1][0].rgbs, blks[2][2][0].rgbs,
                    blks[2][2][1].rgbs, blks[2][1][1].rgbs),
                BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO * BlockAO(
                    blks[2][0][0].rgbs, blks[2][1][0].rgbs,
                    blks[2][1][1].rgbs, blks[2][0][1].rgbs),
                1, model);
    }
    //x-
    if(infoMgr.IsFaceVisible(blk.type, nX.type))
    {
        AddFace({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f },
                { 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f },
                BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO * BlockAO(
                    blks[0][0][0].rgbs, blks[0][1][0].rgbs,
                    blks[0][1][1].rgbs, blks[0][0][1].rgbs),
                BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO * BlockAO(
                    blks[0][1][0].rgbs, blks[0][2][0].rgbs,
                    blks[0][2][1].rgbs, blks[0][1][1].rgbs),
                BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO * BlockAO(
                    blks[0][1][1].rgbs, blks[0][2][1].rgbs,
                    blks[0][2][2].rgbs, blks[0][1][2].rgbs),
                BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO * BlockAO(
                    blks[0][0][1].rgbs, blks[0][1][1].rgbs,
                    blks[0][1][2].rgbs, blks[0][0][2].rgbs),
                2, model);
    }
    //y+
    if(infoMgr.IsFaceVisible(blk.type, pY.type))
    {
        AddFace({ 0.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f },
                { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f },
                BlockAO(blks[0][2][2].rgbs, blks[0][2][1].rgbs, blks[1][2][1].rgbs, blks[1][2][2].rgbs),
                BlockAO(blks[0][2][1].rgbs, blks[0][2][0].rgbs, blks[1][2][0].rgbs, blks[1][2][1].rgbs),
                BlockAO(blks[1][2][1].rgbs, blks[1][2][0].rgbs, blks[2][2][0].rgbs, blks[2][2][1].rgbs),
                BlockAO(blks[1][2][2].rgbs, blks[1][2][1].rgbs, blks[2][2][1].rgbs, blks[2][2][2].rgbs),
                3, model);
    }
    //y-
    if(infoMgr.IsFaceVisible(blk.type, nY.type))
    {
        AddFace({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f },
                { 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f },
                BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO * BlockAO(
                    blks[0][0][1].rgbs, blks[0][0][0].rgbs,
                    blks[1][0][0].rgbs, blks[1][0][1].rgbs),
                BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO * BlockAO(
                    blks[0][0][2].rgbs, blks[0][0][1].rgbs,
                    blks[1][0][1].rgbs, blks[1][0][2].rgbs),
                BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO * BlockAO(
                    blks[1][0][2].rgbs, blks[1][0][1].rgbs,
                    blks[2][0][1].rgbs, blks[2][0][2].rgbs),
                BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO * BlockAO(
                    blks[1][0][1].rgbs, blks[1][0][0].rgbs,
                    blks[2][0][0].rgbs, blks[2][0][1].rgbs),
                4, model);
    }
    //z+
    if(infoMgr.IsFaceVisible(blk.type, pZ.type))
    {
        AddFace({ 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f },
                { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 1.0f },
                BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO * BlockAO(
                    blks[0][0][2].rgbs, blks[0][1][2].rgbs,
                    blks[1][1][2].rgbs, blks[1][0][2].rgbs),
                BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO * BlockAO(
                    blks[0][1][2].rgbs, blks[0][2][2].rgbs,
                    blks[1][2][2].rgbs, blks[1][1][2].rgbs),
                BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO * BlockAO(
                    blks[1][1][2].rgbs, blks[1][2][2].rgbs,
                    blks[2][2][2].rgbs, blks[2][1][2].rgbs),
                BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO * BlockAO(
                    blks[1][0][2].rgbs, blks[1][1][2].rgbs,
                    blks[2][1][2].rgbs, blks[2][0][2].rgbs),
                5, model);
    }
    //z-
    if(infoMgr.IsFaceVisible(blk.type, nZ.type))
    {
        AddFace({ 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 0.0f },
                { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f },
                BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO * BlockAO(
                    blks[1][0][0].rgbs, blks[1][1][0].rgbs,
                    blks[2][1][0].rgbs, blks[2][0][0].rgbs),
                BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO * BlockAO(
                    blks[1][1][0].rgbs, blks[1][2][0].rgbs,
                    blks[2][2][0].rgbs, blks[2][1][0].rgbs),
                BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO * BlockAO(
                    blks[0][1][0].rgbs, blks[0][2][0].rgbs,
                    blks[1][2][0].rgbs, blks[1][1][0].rgbs),
                BLOCK_SIDE_BOTTOM_LIGHT_DEC_RATIO * BlockAO(
                    blks[0][0][0].rgbs, blks[0][1][0].rgbs,
                    blks[1][1][0].rgbs, blks[1][0][0].rgbs),
                6, model);
    }
}

void BlockModelBuilder_CarveRenderer_Cross::Build(
    const Vector3 &posOffset,
    const Block(&blks)[3][3][3],
    ChunkSectionModels *models) const
{
    assert(models != nullptr);

    const Block &blk = blks[1][1][1],
                &pX  = blks[2][1][1],
                &nX  = blks[0][1][1],
                &pY  = blks[1][2][1],
                &nY  = blks[1][0][1],
                &pZ  = blks[1][1][2],
                &nZ  = blks[1][1][0];

    BlockInfoManager &infoMgr = BlockInfoManager::GetInstance();
    const BlockInfo &info = infoMgr.GetBlockInfo(blk.type);

    constexpr float TEX_GRID_SIZE = 1.0f / CARVE_RENDERER_TEXTURE_BLOCK_SIZE;
    CarveModel &model = models->carve[info.carveCrossTexPos[0]];

    if(!infoMgr.IsSolid(pX.type) || !infoMgr.IsSolid(nX.type) ||
       !infoMgr.IsSolid(pY.type) || !infoMgr.IsSolid(nY.type) ||
       !infoMgr.IsSolid(pZ.type) || !infoMgr.IsSolid(nZ.type))
    {
        auto AddFace = [&](const Vector3 &vtx0, const Vector3 &vtx1,
                           const Vector3 &vtx2, const Vector3 &vtx3,
                           const Color &rgbs,
                           int carveCrossTexPos,
                           CarveModel &output)
        {
            float texBaseU = info.carveBoxTexPos[carveCrossTexPos]
                % CARVE_RENDERER_TEXTURE_BLOCK_SIZE * TEX_GRID_SIZE;
            float texBaseV = info.carveBoxTexPos[carveCrossTexPos]
                / CARVE_RENDERER_TEXTURE_BLOCK_SIZE * TEX_GRID_SIZE;
            UINT16 idxStart = static_cast<UINT16>(output.GetVerticesCount());

            output.AddVertex({
                posOffset + vtx0,
                { texBaseU + 0.0015f, texBaseV + TEX_GRID_SIZE - 0.0015f },
                { rgbs.R(), rgbs.G(), rgbs.B() }, rgbs.A()
            });
            output.AddVertex({
                posOffset + vtx1,
                { texBaseU + 0.0015f, texBaseV + 0.0015f },
                { rgbs.R(), rgbs.G(), rgbs.B() }, rgbs.A()
            });
            output.AddVertex({
                posOffset + vtx2,
                { texBaseU + TEX_GRID_SIZE - 0.0015f, texBaseV + 0.0015f },
                { rgbs.R(), rgbs.G(), rgbs.B() }, rgbs.A()
            });
            output.AddVertex({
                posOffset + vtx3,
                { texBaseU + TEX_GRID_SIZE - 0.0015f, texBaseV + TEX_GRID_SIZE - 0.0015f },
                { rgbs.R(), rgbs.G(), rgbs.B() }, rgbs.A()
            });

            output.AddIndex(idxStart);
            output.AddIndex(idxStart + 1);
            output.AddIndex(idxStart + 2);

            output.AddIndex(idxStart);
            output.AddIndex(idxStart + 2);
            output.AddIndex(idxStart + 3);
        };

        AddFace({ 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f },
                { 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f },
                LightToRGBA(blk.rgbs), 1, model);

        AddFace({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f },
                { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 1.0f },
                LightToRGBA(blk.rgbs), 2, model);
    }
}
