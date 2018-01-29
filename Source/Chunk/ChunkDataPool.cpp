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
        static_assert(std::is_trivially_copyable_v<Chunk::BlockTypeData>,
            "Chunk::BlockTypeData shall be trivially copiable");
        static_assert(std::is_trivially_copyable_v<Chunk::BlockLightData>,
            "Chunk::BlockLightData shall be trivially copiable");
        static_assert(std::is_trivially_copyable_v<Chunk::HeightMap>,
            "Chunk::HeightMap shall be trivially copiable");

        assert(rt != nullptr);
        std::memcpy(ck.blocks, rt->blocks, sizeof(Chunk::BlockTypeData));
        std::memcpy(ck.lights, rt->lights, sizeof(Chunk::BlockLightData));
        std::memcpy(ck.heightMap, rt->heightMap, sizeof(Chunk::HeightMap));

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
