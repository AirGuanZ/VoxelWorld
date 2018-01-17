/*================================================================
Filename: BlockInfo.h
Date: 2018.1.17
Created by AirGuanZ
================================================================*/
#ifndef VW_BLOCK_INFO_H
#define VW_BLOCK_INFO_H

#include <cstdint>
#include <vector>

enum class BlockType : std::uint8_t
{
    Air             = 0,
    Bedrock         = Air + 1,
    Stone           = Bedrock + 1,
    Dirt            = Stone + 1,
    Grass           = Dirt + 1,

    BlockTypeNum    = Grass + 1
};

enum class BlockShape : std::uint8_t
{
    Null            = 0,
    Box             = Null + 1,
};

enum class BlockRenderer : std::uint8_t
{
    Null            = 0,
    BasicRenderer   = Null + 1,
};

template<typename Dst = int>
inline constexpr Dst Blk2Int(BlockType blockType)
{
    return static_cast<Dst>(blockType);
}

inline constexpr BlockType Int2Blk(int intValue)
{
    return static_cast<BlockType>(intValue);
}

struct BlockInfo
{
    BlockType type;
    std::string name;

    BlockShape shape;
    BlockRenderer renderer;
};

#endif //VW_BLOCK_INFO_H
