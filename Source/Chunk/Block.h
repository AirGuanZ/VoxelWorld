/*================================================================
Filename: Block.h
Date: 2018.1.15
Created by AirGuanZ
================================================================*/
#ifndef VW_BLOCK_H
#define VW_BLOCK_H

enum BlockType : int
{
    Ukn         = -1,
    Air         = 0,
    Bedrock     = Air + 1,
    Stone       = Bedrock + 1,
    Dirt        = Stone + 1,
    Grass       = Dirt + 1
};

inline int Blk2Int(BlockType blockType)
{
    return static_cast<int>(blockType);
}

inline BlockType Int2Blk(int intValue)
{
    return static_cast<BlockType>(intValue);
}

struct Block
{
    BlockType type = BlockType::Air;
};

static_assert(std::is_trivially_copyable_v<Block>, "Block type must be trivially copiable");

#endif //VW_BLOCK_H
