/*================================================================
Filename: Block.h
Date: 2018.1.15
Created by AirGuanZ
================================================================*/
#ifndef VW_BLOCK_H
#define VW_BLOCK_H

enum BlockType : int
{
    Unknown = -1,
    Air     = 0,
    Stone   = Air + 1,
    Dirt    = Stone + 1,
    Grass   = Dirt + 1
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
    BlockType type;
};

#endif //VW_BLOCK_H
