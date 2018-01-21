/*================================================================
Filename: BlockInfoManager.cpp
Date: 2018.1.17
Created by AirGuanZ
================================================================*/
#include <cassert>
#include <string>

#include "../Resource/ResourceName.h"
#include "../Utility/HelperFunctions.h"
#include "BlockInfoManager.h"

SINGLETON_CLASS_DEFINITION(BlockInfoManager);

/*
    BasicRendererTexture0
        0, Bedrock
        1, Stone
        2, Dirt,
        3, HalfGrass
        4. Grass
*/

//IMPROVE：硬编码改为从配置文件中加载
BlockInfoManager::BlockInfoManager(void)
{
    info_ = {
        {
            BlockType::Air,
            "Air",
            BlockShape::Null,
            BlockRenderer::Null
        },
        {
            BlockType::Bedrock,
            "Bedrock",
            BlockShape::Box,
            BlockRenderer::BasicRenderer
        },
        {
            BlockType::Stone,
            "Stone",
            BlockShape::Box,
            BlockRenderer::BasicRenderer
        },
        {
            BlockType::Dirt,
            "Dirt",
            BlockShape::Box,
            BlockRenderer::BasicRenderer
        },
        {
            BlockType::Grass,
            "Grass",
            BlockShape::Box,
            BlockRenderer::BasicRenderer
        }
    };

    auto SetBasicBoxTexPos = [&](
        BlockType type, int _0, int _1, int _2, int _3, int _4, int _5, int _6)
    {
        int (&a)[7] = info_[Blk2Int(type)].basicBoxTexPos;
        a[0] = _0;
        a[1] = _1, a[2] = _2, a[3] = _3;
        a[4] = _4, a[5] = _5, a[6] = _6;
    };

    SetBasicBoxTexPos(BlockType::Bedrock, 0, 0, 0, 0, 0, 0, 0);
    SetBasicBoxTexPos(BlockType::Stone,   0, 1, 1, 1, 1, 1, 1);
    SetBasicBoxTexPos(BlockType::Dirt,    0, 2, 2, 2, 2, 2, 2);
    SetBasicBoxTexPos(BlockType::Grass,   0, 3, 3, 4, 2, 3, 3);
}

const BlockInfo &BlockInfoManager::GetBlockInfo(BlockType type) const
{
    assert(Blk2Int<size_t>(type) < info_.size());
    return info_[Blk2Int(type)];
}

bool BlockInfoManager::IsFaceVisible(BlockType dst, BlockType neighbor) const
{
    return dst != BlockType::Air && neighbor == BlockType::Air;
}
