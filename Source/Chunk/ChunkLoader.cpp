/*================================================================
Filename: ChunkLoader.cpp
Date: 2018.1.19
Created by AirGuanZ
================================================================*/
#include <algorithm>
#include <cassert>

#include "../Land/LandGenerator_V0.h"
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
        threadNum = (std::max)(4u, std::thread::hardware_concurrency()) - 3;

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
    threads_.clear();

    while(loaderTasks_.size())
    {
        ChunkLoaderTask *task = loaderTasks_.front();
        loaderTasks_.pop();
        Helper::SafeDeleteObjects(task);
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
    return std::move(loaderMsgs_);
}

void ChunkLoader::LoadChunkData(Chunk *ck)
{
    //TODO
    LandGenerator_V0(145).GenerateLand(ck);
}

void ChunkLoader::TaskThreadEntry(void)
{
    while(running_)
    {
        ChunkLoaderTask *task = nullptr;

        {
            std::lock_guard<std::mutex> lk(taskQueueMutex_);
            if(loaderTasks_.size())
            {
                task = loaderTasks_.front();
                loaderTasks_.pop();
            }
        }

        if(task)
        {
            task->Run(this);
            Helper::SafeDeleteObjects(task);
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
    ck_ = nullptr;
    loader->AddMsg(msg);
}

ChunkLoaderTask_LoadChunkData::~ChunkLoaderTask_LoadChunkData(void)
{
    Helper::SafeDeleteObjects(ck_);
}
