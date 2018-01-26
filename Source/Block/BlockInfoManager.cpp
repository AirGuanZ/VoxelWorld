/*================================================================
Filename: BlockInfoManager.cpp
Date: 2018.1.17
Created by AirGuanZ
================================================================*/
#include <cassert>
#include <string>

#include "../Resource/ResourceName.h"
#include "../Utility/HelperFunctions.h"
#include "Block.h"
#include "BlockInfoManager.h"

SINGLETON_CLASS_DEFINITION(BlockInfoManager);

/*
    BasicRendererTexture0
        0, Bedrock
        1, Stone
        2, Dirt,
        3, HalfGrass
        4. GrassBox
        5. Wood T/D
        6. Wood Side
*/

//IMPROVE：硬编码改为从配置文件中加载
BlockInfoManager::BlockInfoManager(void)
{
    info_ = {
        {
            BlockType::Air,
            "Air",
            BlockShape::Null,
            BlockRenderer::Null,
            false, false,
            1, { 0, 0, 0 }
        },
        {
            BlockType::Bedrock,
            "Bedrock",
            BlockShape::Box,
            BlockRenderer::BasicRenderer,
            true, true,
            1000, { 0, 0, 0 }
        },
        {
            BlockType::Stone,
            "Stone",
            BlockShape::Box,
            BlockRenderer::BasicRenderer,
            true, true,
            1000, { 0, 0, 0 }
        },
        {
            BlockType::Dirt,
            "Dirt",
            BlockShape::Box,
            BlockRenderer::BasicRenderer,
            true, true,
            1000, { 0, 0, 0 }
        },
        {
            BlockType::GrassBox,
            "GrassBox",
            BlockShape::Box,
            BlockRenderer::BasicRenderer,
            true, true,
            1000, { 0, 0, 0 }
        },
        {
            BlockType::Sand,
            "Sand",
            BlockShape::Box,
            BlockRenderer::BasicRenderer,
            true, true,
            1000, { 0, 0, 0 }
        },
        {
            BlockType::Wood,
            "Wood",
            BlockShape::Box,
            BlockRenderer::BasicRenderer,
            true, true,
            1000, { 0, 0, 0 }
        },
        {
            BlockType::Leaf,
            "Leaf",
            BlockShape::Box,
            BlockRenderer::CarveRenderer,
            false, true,
            2, { 0, 0, 0 }
        },
        {
            BlockType::GlowStone,
            "GlowStone",
            BlockShape::Box,
            BlockRenderer::BasicRenderer,
            true, true,
            1, { 0, LIGHT_COMPONENT_MAX, LIGHT_COMPONENT_MAX }
        },
        {
            BlockType::Grass,
            "Grass",
            BlockShape::Cross,
            BlockRenderer::CarveRenderer,
            false, true,
            1, { 0, 0, 0 }
        },
        {
            BlockType::Flower,
            "Flower",
            BlockShape::Cross,
            BlockRenderer::CarveRenderer,
            false, true,
            1, { 0, 0, 0 }
        },
    };

    auto SetBasicBoxTexPos = [&](
        BlockType type, int _0, int _1, int _2, int _3, int _4, int _5, int _6)
    {
        int (&a)[7] = info_[Blk2Int(type)].basicBoxTexPos;
        a[0] = _0;
        a[1] = _1, a[2] = _2, a[3] = _3;
        a[4] = _4, a[5] = _5, a[6] = _6;
    };

    auto SetCarveBoxTexPos = [&](
        BlockType type, int _0, int _1, int _2, int _3, int _4, int _5, int _6)
    {
        int (&a)[7] = info_[Blk2Int(type)].carveBoxTexPos;
        a[0] = _0;
        a[1] = _1, a[2] = _2, a[3] = _3;
        a[4] = _4, a[5] = _5, a[6] = _6;
    };

    auto SetCarveCrossTexPos = [&](
        BlockType type, int _0, int _1, int _2)
    {
        int (&a)[3] = info_[Blk2Int(type)].carveCrossTexPos;
        a[0] = _0;
        a[1] = _1, a[2] = _2;
    };

    SetBasicBoxTexPos(BlockType::Bedrock,   0, 0, 0, 0, 0, 0, 0);
    SetBasicBoxTexPos(BlockType::Stone,     0, 1, 1, 1, 1, 1, 1);
    SetBasicBoxTexPos(BlockType::Dirt,      0, 2, 2, 2, 2, 2, 2);
    SetBasicBoxTexPos(BlockType::GrassBox,  0, 3, 3, 4, 2, 3, 3);
    SetBasicBoxTexPos(BlockType::Sand,      0, 8, 8, 8, 8, 8, 8);
    SetBasicBoxTexPos(BlockType::Wood,      0, 6, 6, 5, 5, 6, 6);
    SetCarveBoxTexPos(BlockType::Leaf,      0, 0, 0, 0, 0, 0, 0);
    SetBasicBoxTexPos(BlockType::GlowStone, 0, 7, 7, 7, 7, 7, 7);
    SetCarveCrossTexPos(BlockType::Grass,   0, 1, 1);
    SetCarveCrossTexPos(BlockType::Flower,  0, 2, 2);
}

const BlockInfo &BlockInfoManager::GetBlockInfo(BlockType type) const
{
    assert(Blk2Int<size_t>(type) < info_.size());
    return info_[Blk2Int(type)];
}

bool BlockInfoManager::IsFaceVisible(BlockType dst, BlockType neighbor) const
{
    const BlockInfo &infoDst = info_[Blk2Int(dst)], infoNei = info_[Blk2Int(neighbor)];
    if(infoDst.renderer == BlockRenderer::Null)
        return false;
    if(infoNei.renderer == BlockRenderer::Null)
        return true;
    if(infoNei.renderer == BlockRenderer::CarveRenderer)
        return infoDst.renderer != BlockRenderer::CarveRenderer | dst != neighbor;
    return false;
}

bool BlockInfoManager::IsSolid(BlockType type) const
{
    return info_[Blk2Int(type)].isSolid;
}

bool BlockInfoManager::IsRenderable(BlockType type) const
{
    return info_[Blk2Int(type)].isRenderable;
}
