/*================================================================
Filename: ChunkManager.cpp
Date: 2018.1.18
Created by AirGuanZ
================================================================*/
#include <cstdlib>
#include <mutex>

#include "../Utility/HelperFunctions.h"
#include "ChunkLoader.h"
#include "ChunkManager.h"

ChunkManager::ChunkManager(int loadDistance, int renderDistance)
    : loadDistance_(loadDistance), renderDistance_(renderDistance)
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

bool ChunkManager::InRenderRange(int ckX, int ckZ)
{
    return std::abs(ckX - centrePos_.x) <= renderDistance_ &&
           std::abs(ckZ - centrePos_.z) <= renderDistance_;
}

bool ChunkManager::InLoadingRange(int ckX, int ckZ)
{
    return std::abs(ckX - centrePos_.x) <= loadDistance_ &&
           std::abs(ckZ - centrePos_.z) <= loadDistance_;
}

void ChunkManager::SetCentrePosition(int ckX, int ckZ)
{
    if(centrePos_.x == ckX && centrePos_.z == ckZ)
        return;
    centrePos_ = { ckX, ckZ };

    //干掉出了范围的Chunk
    decltype(chunks_) newChunks_;
    for(auto it : chunks_)
    {
        if(!InLoadingRange(it.first.x, it.first.z))
            ckLoader_.AddTask(new ChunkLoaderTask_DestroyChunk(it.second));
        else
            newChunks_[it.first] = it.second;
    }
    chunks_ = std::move(newChunks_);

    //遍历新范围内的Chunk
    //缺数据的建立加载任务
    //不缺数据的看看需不需要建立模型任务
    int loadRangeXEnd = centrePos_.x + loadDistance_;
    int loadRangeZEnd = centrePos_.z + loadDistance_;
    for(int newCkX = centrePos_.x - loadDistance_; newCkX <= loadRangeXEnd; ++newCkX)
    {
        for(int newCkZ = centrePos_.z - loadDistance_; newCkZ <= loadRangeZEnd; ++newCkZ)
        {
            auto it = chunks_.find({ newCkZ, newCkZ });
            if(it == chunks_.end()) //还没有这个区块的数据
                ckLoader_.AddTask(
                    new ChunkLoaderTask_LoadChunkData(new Chunk(this, { newCkX, newCkZ })));
            else if(InRenderRange(newCkX, newCkZ)) //有数据了，看看在不在渲染范围内
            {
                for(int section = 0; section != CHUNK_SECTION_NUM; ++section)
                {
                    if(!it->second->GetModels(section))
                        unimportantModelUpdates_.insert({ newCkX, section, newCkZ });
                }
            }
        }
    }
}

void ChunkManager::AddChunkData(Chunk *ck)
{
    assert(ck != nullptr);
    decltype(chunks_)::iterator it;
    IntVectorXZ pos = ck->GetPosition();

    //不在加载范围内/已经有了，所以直接丢掉
    if(!InLoadingRange(pos.x, pos.z) ||
       (it = chunks_.find(pos)) != chunks_.end())
    {
        ckLoader_.AddTask(new ChunkLoaderTask_DestroyChunk(ck));
        return;
    }

    chunks_[pos] = ck;
    if(InRenderRange(pos.x, pos.z)) //是否需要创建模型任务
    {
        for(int section = 0; section != CHUNK_SECTION_NUM; ++section)
            unimportantModelUpdates_.insert({ pos.x, section, pos.z });
    }
}

void ChunkManager::LoadChunk(int ckX, int ckZ)
{
    assert(chunks_.find({ ckX, ckZ }) == chunks_.end());

    Chunk *ck = new Chunk(this, { ckX, ckZ });
    ckLoader_.LoadChunkData(ck);
    AddChunkData(ck);
}

void ChunkManager::ProcessChunkLoaderMessages(void)
{
    std::queue<ChunkLoaderMessage*> msgs = ckLoader_.FetchAllMsgs();
    while(!msgs.empty())
    {
        ChunkLoaderMessage *msg = msgs.front();
        msgs.pop();

        switch(msg->type)
        {
        case ChunkLoaderMessage::ChunkLoaded:
            AddChunkData(msg->ckLoaded);
            break;
        default:
            std::abort();
        }

        Helper::SafeDeleteObjects(msg);
    }
}
