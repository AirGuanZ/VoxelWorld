/*================================================================
Filename: ChunkLoader.h
Date: 2018.1.19
Created by AirGuanZ
================================================================*/
#ifndef VW_CHUNK_LOADER_H
#define VW_CHUNK_LOADER_H

#include <atomic>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "Chunk.h"
#include "ChunkDataPool.h"

/*
一个后台加载任务分为两个阶段
    1. 数据加载
    2. 光照预计算

据此，ChunkLoader除了有一个基本的加载区块的任务队列，还应维护一个用于
光照预计算的区块池，池子中的区块都是加载好了数据、仅进行了天光直射计算的区块。

池子基于LRU策略管理，单独封装好，提供取得区块数据的操作。

Loader和池子都要支持直接在主线程加载、计算数据，这样当主线程立即要求数据的时候不至于要阻塞。
*/

class ChunkLoader;

class ChunkLoaderTask
{
public:
    virtual void Run(ChunkLoader *loader) = 0;
    virtual ~ChunkLoaderTask(void) { }
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
        std::queue<ChunkLoaderTask*> newTasks;
        while(loaderTasks_.size())
        {
            ChunkLoaderTask *task = loaderTasks_.front();
            loaderTasks_.pop();
            if(func(task))
                newTasks.push(task);
            else
                Helper::SafeDeleteObjects(task);
        }
        loaderTasks_ = std::move(newTasks);
    }

    //真正的区块数据加载函数
    //和线程没啥关系
    void LoadChunkData(Chunk *ck);

private:
    void TaskThreadEntry(void);

private:
    ChunkDataPool ckPool_;

    std::vector<std::thread> threads_;
    std::atomic<bool> running_;

    std::queue<ChunkLoaderTask*> loaderTasks_;
    std::queue<ChunkLoaderMessage*> loaderMsgs_;

    std::mutex taskQueueMutex_;
    std::mutex msgQueueMutex_;
};

class ChunkLoaderTask_LoadChunkData : public ChunkLoaderTask
{
public:
    ChunkLoaderTask_LoadChunkData(Chunk *ck);
    ~ChunkLoaderTask_LoadChunkData(void);

    void Run(ChunkLoader *loader);

    IntVectorXZ GetPosition(void)
    {
        return ck_->GetPosition();
    }

private:
    Chunk *ck_;
};

#endif //VW_CHUNK_LOADER_H
