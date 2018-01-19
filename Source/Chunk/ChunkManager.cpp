/*================================================================
Filename: ChunkManager.cpp
Date: 2018.1.18
Created by AirGuanZ
================================================================*/
#include <mutex>

#include "ChunkManager.h"

ChunkManager::ChunkManager(void)
{

}

ChunkManager::~ChunkManager(void)
{
    assert(chunks_.empty());
}

Chunk *ChunkManager::GetChunk(int ckX, int ckZ)
{
    auto it = chunks_.find({ ckX, ckZ });
    if(it != chunks_.end())
        return it->second;

    LoadChunk(ckX, ckZ);
    return chunks_[{ ckX, ckZ }];
}

Block &ChunkManager::GetBlock(int x, int y, int z)
{
    if(y < 0 || y >= CHUNK_MAX_HEIGHT)
    {
        static Block dummyAir;
        dummyAir = Block();
        return dummyAir;
    }
    return GetChunk(BlockXZ_To_ChunkXZ(x), BlockXZ_To_ChunkXZ(z))
        ->GetBlock(BlockXZ_To_BlockXZInChunk(x),
                   y,
                   BlockXZ_To_BlockXZInChunk(z));
}

void ChunkManager::SetBlock(int x, int y, int z, const Block &blk)
{
    if(y < 0 || y >= CHUNK_MAX_HEIGHT)
        return;
    GetChunk(BlockXZ_To_ChunkXZ(x), BlockXZ_To_ChunkXZ(z))
        ->SetBlock(BlockXZ_To_BlockXZInChunk(x),
                   y,
                   BlockXZ_To_BlockXZInChunk(z),
                   blk);
}

void ChunkManager::LoadChunk(int ckX, int ckZ)
{
    //TODO
}
