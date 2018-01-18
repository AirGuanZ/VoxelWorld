/*================================================================
Filename: ChunkManager.cpp
Date: 2018.1.16
Created by AirGuanZ
================================================================*/
#include "ChunkManager.h"

ChunkManager::ChunkManager(void)
{

}

ChunkManager::~ChunkManager(void)
{

}

void ChunkManager::StartChunkLoading(void)
{
    ckLoader_.Initialize(-1);
}

void ChunkManager::EndChunkLoading(void)
{
    ckLoader_.Destroy();
}

Chunk *ChunkManager::GetChunkByBlockPos(int x, int z)
{
    IntVector2XZ xz = BlockPosToChunkPos({ x, z });
    return GetChunkByChunkPos(xz.x, xz.z);
}

Chunk *ChunkManager::GetChunkByChunkPos(int x, int z)
{
    auto &rt = cks_.find({ x, z });
    return rt == cks_.end() ? nullptr : rt->second;
}
