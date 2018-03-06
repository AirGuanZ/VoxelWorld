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
һ����̨���������Ϊ�����׶�
    1. ���ݼ���
    2. ����Ԥ����
    3. ģ�ʹ���

�ݴˣ�ChunkLoader������һ�������ļ��������������У���Ӧά��һ������
����Ԥ���������أ������е����鶼�Ǽ��غ������ݡ������������ֱ���������顣

���ӻ���LRU���Թ���������װ�ã��ṩȡ���������ݵĲ�����

Loader�ͳ��Ӷ�Ҫ֧��ֱ�������̼߳��ء��������ݣ����������߳�����Ҫ�����ݵ�ʱ������Ҫ������
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

    //�������������ݼ��غ���
    //�߳��޹�
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
