/*================================================================
Filename: ChunkDataPool.cpp
Date: 2018.1.29
Created by AirGuanZ
================================================================*/
#include <cassert>
#include <cstring>
#include <type_traits>

#include "../Utility/HelperFunctions.h"
#include "ChunkDataPool.h"

ChunkDataPool::ChunkDataPool(size_t maxDataCnt)
    : maxDataCnt_(maxDataCnt)
{

}

ChunkDataPool::~ChunkDataPool(void)
{
    Destroy();
}

void ChunkDataPool::Destroy(void)
{
    std::lock_guard<std::mutex> lk(mapMutex_);
    map_.ForEach([](Chunk *ck) { Helper::SafeDeleteObjects(ck); });
    map_.Clear();
}

bool ChunkDataPool::GetChunk(Chunk &ck)
{
    IntVectorXZ pos = ck.GetPosition();
    std::lock_guard<std::mutex> lk(mapMutex_);
    Chunk *rt = nullptr;
    if(map_.FindAndErase({ pos.x, pos.z }, rt))
    {
        assert(rt != nullptr);

        CopyChunkData(ck, *rt);

        map_.PushFront(pos, rt);
        return true;
    }
    return false;
}

void ChunkDataPool::AddChunk(Chunk *ck)
{
    assert(ck != nullptr);
    IntVectorXZ pos = ck->GetPosition();
    std::lock_guard<std::mutex> lk(mapMutex_);

    Chunk *existed = nullptr;
    if(map_.FindAndErase(pos, existed))
    {
        map_.PushFront(pos, ck);
        Helper::SafeDeleteObjects(existed);
        return;
    }

    map_.PushFront(pos, ck);
    while(map_.Size() > maxDataCnt_)
    {
        Helper::SafeDeleteObjects(map_.Back());
        map_.PopBack();
    }
}
