/*================================================================
Filename: ChunkManager.h
Date: 2018.1.18
Created by AirGuanZ
================================================================*/
#ifndef VW_CHUNK_MANAGER_H
#define VW_CHUNK_MANAGER_H

#include <mutex>
#include <string>
#include <unordered_map>

#include "../Utility/Math.h"
#include "../Utility/Uncopiable.h"
#include "Chunk.h"

class ChunkManager
{
public:
    ChunkManager(void);
    ~ChunkManager(void);

    //返回的Chunk在本帧内绝不会失效
    //仅主线程调用
    Chunk *GetChunk(int ckX, int ckZ);

    //仅主线程调用
    Block &GetBlock(int blkX, int blkY, int blkZ);
    void SetBlock(int blkX, int blkY, int blkZ, const Block &blk);

private:

    std::unordered_map<IntVectorXZ, Chunk*> chunks_;
};

inline int BlockXZ_To_ChunkXZ(int blk)
{
    return (blk + 0x40000000) / CHUNK_SECTION_SIZE + 0x40000000 / CHUNK_SECTION_SIZE;
}

inline int BlockXZ_To_BlockXZInChunk(int blk)
{
    return (blk + 0x40000000) % CHUNK_SECTION_SIZE;
}

inline int BlockY_To_ChunkSectionIndex(int blk)
{
    return blk / CHUNK_SECTION_SIZE;
}

inline int BlockY_To_BlockYInChunkSection(int blk)
{
    return blk % CHUNK_SECTION_SIZE;
}

inline int ChunkXZ_To_BlockXZ(int ck)
{
    return ck * CHUNK_SECTION_SIZE;
}

inline int ChunkSectionIndex_To_BlockY(int cks)
{
    return cks * CHUNK_SECTION_SIZE;
}

#endif //VW_CHUNK_MANAGER_H
