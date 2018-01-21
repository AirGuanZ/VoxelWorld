/*================================================================
Filename: ChunkManager.cpp
Date: 2018.1.18
Created by AirGuanZ
================================================================*/
#include <cstdlib>
#include <limits>
#include <mutex>

#include "../Utility/HelperFunctions.h"
#include "ChunkLoader.h"
#include "ChunkManager.h"
#include "ChunkModelBuilder.h"

ChunkManager::ChunkManager(int loadDistance,
                           int renderDistance,
                           int maxImpModelUpdates,
                           int maxUniModelUpdates,
                           int maxModelUpdates)
    : loadDistance_(loadDistance),
      renderDistance_(renderDistance),
      maxImpModelUpdates_(maxImpModelUpdates),
      maxUniModelUpdates_(maxUniModelUpdates),
      maxModelUpdates_(maxModelUpdates)
{
    centrePos_.x = (std::numeric_limits<decltype(centrePos_.x)>::min)();
    centrePos_.z = (std::numeric_limits<decltype(centrePos_.z)>::min)();
}

ChunkManager::~ChunkManager(void)
{
    Destroy();
}

void ChunkManager::StartLoading(void)
{
    ckLoader_.Initialize();
}

void ChunkManager::Destroy(void)
{
    ckLoader_.Destroy();

    std::queue<ChunkLoaderMessage*> msgs = ckLoader_.FetchAllMsgs();
    while(!msgs.empty())
    {
        ChunkLoaderMessage *msg = msgs.front();
        msgs.pop();

        switch(msg->type)
        {
        case ChunkLoaderMessage::ChunkLoaded:
            delete msg->ckLoaded;
            break;
        default:
            std::abort();
        }

        delete msg;
    }

    for(auto it : chunks_)
        Helper::SafeDeleteObjects(it.second);

    chunks_.clear();
    importantModelUpdates_.clear();
    unimportantModelUpdates_.clear();
}

Chunk *ChunkManager::GetChunk(int ckX, int ckZ)
{
    auto it = chunks_.find({ ckX, ckZ });
    if(it != chunks_.end())
        return it->second;

    LoadChunk(ckX, ckZ);
    assert((chunks_[{ ckX, ckZ }] != nullptr));
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
    return centrePos_.x - loadDistance_ <= ckX && ckX <= centrePos_.x + loadDistance_ &&
           centrePos_.z - loadDistance_ <= ckZ && ckZ <= centrePos_.z + loadDistance_;
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
            delete it.second;
        else
            newChunks_[it.first] = it.second;
    }
    chunks_.swap(newChunks_);

    //遍历新范围内的Chunk
    //缺数据的建立加载任务
    //不缺数据的看看需不需要建立模型任务

    int loadRangeXEnd = centrePos_.x + loadDistance_;
    int loadRangeZEnd = centrePos_.z + loadDistance_;
    for(int newCkX = centrePos_.x - loadDistance_; newCkX <= loadRangeXEnd; ++newCkX)
    {
        for(int newCkZ = centrePos_.z - loadDistance_; newCkZ <= loadRangeZEnd; ++newCkZ)
        {
            assert(InLoadingRange(newCkX, newCkZ));
            auto it = chunks_.find({ newCkX, newCkZ });
            if(it == chunks_.end()) //还没有这个区块的数据
            {
                ckLoader_.AddTask(
                    new ChunkLoaderTask_LoadChunkData(new Chunk(this, { newCkX, newCkZ })));
            }
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

void ChunkManager::MakeSectionModelInvalid(int x, int y, int z)
{
    auto it = chunks_.find({ x, z });
    if(it == chunks_.end())
        return;
    importantModelUpdates_.insert({ x, y, z });
}

void ChunkManager::AddChunkData(Chunk *ck)
{
    assert(ck != nullptr);
    decltype(chunks_)::iterator it;
    IntVectorXZ pos = ck->GetPosition();

    //已经有了，所以直接丢掉
    if((it = chunks_.find(pos)) != chunks_.end() && it->second)
    {
        delete ck;
        return;
    }

    chunks_[pos] = ck;
    if(InRenderRange(pos.x, pos.z)) //是否需要创建模型任务
    {
        for(int section = 0; section != CHUNK_SECTION_NUM; ++section)
            unimportantModelUpdates_.insert({ pos.x, section, pos.z });
    }
}

void ChunkManager::AddSectionModel(const IntVector3 &pos, ChunkSectionModels *models)
{
    assert(models != nullptr);
    auto it = chunks_.find({ pos.x, pos.z });
    if(it == chunks_.end())
    {
        Helper::SafeDeleteObjects(models);
        return;
    }
    it->second->SetModel(pos.y, models);
}

void ChunkManager::LoadChunk(int ckX, int ckZ)
{
    assert(chunks_.find({ ckX, ckZ }) == chunks_.end());

    Chunk *ck = new Chunk(this, { ckX, ckZ });
    ckLoader_.LoadChunkData(ck);
    assert(ck != nullptr);
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
            if(InLoadingRange(msg->ckLoaded->GetPosition().x, msg->ckLoaded->GetPosition().z))
                AddChunkData(msg->ckLoaded);
            break;
        default:
            std::abort();
        }

        Helper::SafeDeleteObjects(msg);
    }
}

void ChunkManager::ProcessModelUpdates(void)
{
    //优先处理重要更新
    int updatesCount = 0;
    while(!importantModelUpdates_.empty() && updatesCount < maxImpModelUpdates_)
    {
        IntVector3 pos = *importantModelUpdates_.begin();
        importantModelUpdates_.erase(pos);

        auto it = chunks_.find({ pos.x, pos.z });
        if(it == chunks_.end())
            continue;
        ChunkModelBuilder builder(this, it->second, pos.y);
        AddSectionModel(pos, builder.Build());
        ++updatesCount;
    }

    //然后才处理不重要更新
    int uniUpdatesCount = 0;
    while(!unimportantModelUpdates_.empty() &&
          uniUpdatesCount < maxUniModelUpdates_ &&
          updatesCount < maxModelUpdates_)
    {
        IntVector3 pos = *unimportantModelUpdates_.begin();
        unimportantModelUpdates_.erase(pos);

        auto it = chunks_.find({ pos.x, pos.z });
        if(it == chunks_.end())
            continue;
        ChunkModelBuilder builder(this, it->second, pos.y);
        AddSectionModel(pos, builder.Build());
        ++updatesCount;
        ++uniUpdatesCount;
    }
}

void ChunkManager::Render(ChunkSectionRenderQueue *renderQueue)
{
    assert(renderQueue != nullptr);

    for(auto it : chunks_)
    {
        if(!InRenderRange(it.first.x, it.first.z))
            continue;

        ChunkSectionModels *models;
        for(int section = 0; section != CHUNK_SECTION_NUM; ++section)
        {
            if(models = it.second->GetModels(section))
            {
                for(int i = 0; i != BASIC_RENDERER_TEXTURE_NUM; ++i)
                    renderQueue->basic[i].AddModel(&models->basic[i]);
            }
        }
    }
}
