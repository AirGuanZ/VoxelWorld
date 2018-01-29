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
LRU���������ݣ����Ӵ�СӦ���Դ�����Ҫ����������������

��ҪLRU����Ҫ�����������ڲ���linked hash map
*/
class ChunkDataPool
{
public:

private:
    std::vector<std::thread> threads_;
};

#endif //VW_CHUNK_DATA_POOL_H
