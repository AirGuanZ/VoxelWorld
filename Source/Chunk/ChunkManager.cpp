/*================================================================
Filename: ChunkManager.cpp
Date: 2018.1.18
Created by AirGuanZ
================================================================*/
#include <mutex>

#include "ChunkFile.h"
#include "ChunkManager.h"

ChunkManager::ChunkManager(void)
{

}

ChunkManager::~ChunkManager(void)
{
    assert(chunks_.empty());
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
