/*================================================================
Filename: ChunkLoader.h
Date: 2018.1.19
Created by AirGuanZ
================================================================*/
#pragma once

#include <atomic>
#include <deque>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include <Utility/LinkedMap.h>

#include <Land/LandGenerator_V0.h>
#include <Land/V1/LandGenerator.h>
#include <Land/V2/LandGenerator_V2.h>
#include "Chunk.h"
#include "ChunkDataPool.h"

/*
一个后台加载任务分为两个阶段
    1. 数据加载
    2. 光照预计算
    3. 模型创建

据此，ChunkLoader除了有一个基本的加载区块的任务队列，还应维护一个用于
光照预计算的区块池，池子中的区块都是加载好了数据、仅进行了天光直射计算的区块。

池子基于LRU策略管理，单独封装好，提供取得区块数据的操作。

Loader和池子都要支持直接在主线程加载、计算数据，这样当主线程立即要求数据的时候不至于要阻塞。
*/

class ChunkLoader;

class ChunkLoaderTask
{
public:
    ChunkLoaderTask(Chunk *ck);
    ~ChunkLoaderTask(void);

    void Run(ChunkLoader *loader);

    IntVectorXZ GetPosition(void)
    {
        return ck_->GetPosition();
    }

private:
    Chunk *ck_;
};

struct ChunkLoaderMessage
{
    enum { ChunkLoaded } type;
    union
    {
        Chunk *ckLoaded;
    };
};

class ChunkLoader
{
public:
    ChunkLoader(size_t ckPoolSize);
    ~ChunkLoader(void);

    void Initialize(int threadNum = -1);
    void Destroy(void);

    void AddTask(ChunkLoaderTask *task);
    void AddMsg(ChunkLoaderMessage *msg);

    ChunkLoaderMessage *FetchMsg(void);
    std::queue<ChunkLoaderMessage*> FetchAllMsgs(void);

    template<typename FuncType>
    void DelTaskIf(FuncType &&func)
    {
        std::lock_guard<std::mutex> lk(taskQueueMutex_);
        decltype(loaderTasks_) newTasks;
        loaderTasks_.ForEach([&](ChunkLoaderTask *task)
        {
            if(func(task))
                newTasks.PushFront(task->GetPosition(), task);
            else
                Helper::SafeDeleteObjects(task);
        });
        loaderTasks_ = std::move(newTasks);
    }

    template<typename FuncType>
    void Sort(FuncType &&func)
    {
        std::lock_guard<std::mutex> lk(taskQueueMutex_);
        loaderTasks_.Sort(std::forward<FuncType>(func));
    }

    //真正的区块数据加载函数
    //线程无关
    void LoadChunkData(Chunk *ck);

    void TryAddLoadingTask(ChunkManager *ckMgr, int ckX, int ckZ);

private:
    void TaskThreadEntry(void);

private:
    ChunkDataPool ckPool_;

    std::vector<std::thread> threads_;
    std::atomic<bool> running_;

    LinkedMap<IntVectorXZ, ChunkLoaderTask*> loaderTasks_;
    std::queue<ChunkLoaderMessage*> loaderMsgs_;

    std::mutex taskQueueMutex_;
    std::mutex msgQueueMutex_;

    LandGenerator_V2::LandGenerator landGen_;
};
