/*================================================================
Filename: ChunkLoader.cpp
Date: 2018.1.19
Created by AirGuanZ
================================================================*/
#include <algorithm>
#include <cassert>

#include "../Utility/HelperFunctions.h"
#include "ChunkLoader.h"
#include "ChunkManager.h"

ChunkLoader::~ChunkLoader(void)
{
    Destroy();
}

void ChunkLoader::Initialize(int threadNum)
{
    assert(threads_.empty());
    if(threadNum <= 0)
        threadNum = (std::max)(2u, std::thread::hardware_concurrency()) - 1;

    //IMPROVE：为了防止文件IO冲突，暂时只设为1
    //解决这个问题
    threadNum = 1;

    running_ = true;
    while(threadNum-- > 0)
        threads_.emplace_back(&ChunkLoader::TaskThreadEntry, this);
}

void ChunkLoader::Destroy(void)
{
    running_ = false;
    for(std::thread &th : threads_)
    {
        if(th.joinable())
            th.join();
    }
}

void ChunkLoader::AddTask(ChunkLoaderTask *task)
{
    assert(task != nullptr);

    std::lock_guard<std::mutex> lk(taskQueueMutex_);
    loaderTasks_.push(task);
}

void ChunkLoader::AddMsg(ChunkLoaderMessage *msg)
{
    assert(msg != nullptr);

    std::lock_guard<std::mutex> lk(msgQueueMutex_);
    loaderMsgs_.push(msg);
}

ChunkLoaderMessage *ChunkLoader::FetchMsg(void)
{
    std::lock_guard<std::mutex> lk(msgQueueMutex_);

    if(loaderMsgs_.empty())
        return nullptr;

    ChunkLoaderMessage *rt = loaderMsgs_.front();
    loaderMsgs_.pop();
    return rt;
}

std::queue<ChunkLoaderMessage*> ChunkLoader::FetchAllMsgs(void)
{
    std::lock_guard<std::mutex> lk(msgQueueMutex_);

    std::queue<ChunkLoaderMessage*> rt;
    rt.swap(loaderMsgs_);
    return rt;
}

void ChunkLoader::LoadChunkData(Chunk *ck)
{
    //TODO

    //把Chunk数据下半部分设为Stone，上半部分设为Air
    Chunk::BlockData &data = ck->GetBlockData();
    for(int x = 0; x != CHUNK_SECTION_SIZE; ++x)
    {
        for(int z = 0; z != CHUNK_SECTION_SIZE; ++z)
        {
            for(int y = 0; y != CHUNK_MAX_HEIGHT / 2; ++y)
            {
                Block blk;
                blk.type = BlockType::Stone;
                blk.sunlight = 1.0f;
                blk.lightColor = { 255, 255, 255 };
                data[x][y][z] = blk;
            }

            for(int y = CHUNK_MAX_HEIGHT / 2; y != CHUNK_MAX_HEIGHT; ++y)
            {
                Block blk;
                blk.type = BlockType::Air;
                blk.sunlight = 1.0f;
                blk.lightColor = { 0, 0, 0 };
                data[x][y][z] = blk;
            }
        }
    }
}

void ChunkLoader::TaskThreadEntry(void)
{
    while(running_)
    {
        ChunkLoaderTask *task = nullptr;

        {
            std::lock_guard<std::mutex> lk(taskQueueMutex_);
            if(!loaderTasks_.empty())
            {
                task = loaderTasks_.front();
                loaderTasks_.pop();
            }
        }

        if(task)
        {
            task->Run(this);
            delete task;
        }
        else
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
}

ChunkLoaderTask_LoadChunkData::ChunkLoaderTask_LoadChunkData(Chunk *ck)
    : ck_(ck)
{
    assert(ck != nullptr);
}

void ChunkLoaderTask_LoadChunkData::Run(ChunkLoader *loader)
{
    assert(loader != nullptr);

    loader->LoadChunkData(ck_);

    ChunkLoaderMessage *msg = new ChunkLoaderMessage;
    msg->type = ChunkLoaderMessage::ChunkLoaded;
    msg->ckLoaded = ck_;
    loader->AddMsg(msg);
}

ChunkLoaderTask_DestroyChunk::ChunkLoaderTask_DestroyChunk(Chunk *ck)
    : ck_(ck)
{
    assert(ck != nullptr);
}

void ChunkLoaderTask_DestroyChunk::Run(ChunkLoader *loader)
{
    assert(loader != nullptr);

    for(int section = 0; section != CHUNK_SECTION_NUM; ++section)
        Helper::SafeDeleteObjects(ck_->GetModels(section));
    delete ck_;
}
