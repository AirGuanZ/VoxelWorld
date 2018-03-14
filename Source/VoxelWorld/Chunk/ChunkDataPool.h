/*================================================================
Filename: ChunkDataPool.h
Date: 2018.1.29
Created by AirGuanZ
================================================================*/
#pragma once

#include <mutex>
#include <unordered_map>
#include <vector>

#include <Utility\LinkedMap.h>
#include <Utility\Uncopiable.h>

#include "Chunk.h"

/*
LRU管理缓存数据，池子大小应该略大于需要加载数据区域的外层
*/
class ChunkDataPool : public Uncopiable
{
public:
    ChunkDataPool(size_t maxDataCnt);
    ~ChunkDataPool(void);

    void Destroy(void);

    //IMPROVE：目前拷贝数据会阻塞其他试图来拷数据的线程
    //改成由来要数据的线程自己拷贝的形式
    //不过这恐怕需要较为复杂的同步机制，而且内存就那么一条线，我怀疑会不会负优化
    bool GetChunk(Chunk &ck);

    //如果add之后存储的数据数量超过maxDataCnt，
    //会删除最后一次使用距今最远的数据
    void AddChunk(Chunk *ck);

private:
    size_t maxDataCnt_;

    std::mutex mapMutex_;
    LinkedMap<IntVectorXZ, Chunk*> map_;
};
