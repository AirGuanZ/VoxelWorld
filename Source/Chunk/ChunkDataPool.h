/*================================================================
Filename: ChunkDataPool.h
Date: 2018.1.29
Created by AirGuanZ
================================================================*/
#ifndef VW_CHUNK_DATA_POOL_H
#define VW_CHUNK_DATA_POOL_H

#include <thread>
#include <unordered_map>
#include <vector>

#include "Chunk.h"

/*
LRU管理缓存数据，池子大小应该略大于需要加载数据区域的外层

又要LRU，又要快速索引，内部用linked hash map
*/
class ChunkDataPool
{
public:

private:
    std::vector<std::thread> threads_;
};

#endif //VW_CHUNK_DATA_POOL_H
