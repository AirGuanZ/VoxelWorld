/*================================================================
Filename: Block.h
Date: 2018.1.15
Created by AirGuanZ
================================================================*/
#ifndef VW_BLOCK_H
#define VW_BLOCK_H

#include <type_traits>

#include "BlockInfo.h"

struct BlockLightColor
{
    std::uint8_t r, g, b;
};

struct Block
{
    BlockType type = BlockType::Air;
    //天光和其他光照分开存储
    BlockLightColor lightColor = { 0, 0, 0 };
    float sunlight = 0.0f;
};

static_assert(std::is_trivially_copyable_v<Block>, "Block type must be trivially copiable");

#endif //VW_BLOCK_H
