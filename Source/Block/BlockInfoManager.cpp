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

//IMPROVE��Ӳ�����Ϊ�������ļ��м���
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
