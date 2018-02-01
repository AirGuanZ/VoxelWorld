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
һ����̨���������Ϊ�����׶�
    1. ���ݼ���
    2. ����Ԥ����

�ݴˣ�ChunkLoader������һ�������ļ��������������У���Ӧά��һ������
����Ԥ���������أ������е����鶼�Ǽ��غ������ݡ������������ֱ���������顣

���ӻ���LRU���Թ���������װ�ã��ṩȡ���������ݵĲ�����

Loader�ͳ��Ӷ�Ҫ֧��ֱ�������̼߳��ء��������ݣ����������߳�����Ҫ�����ݵ�ʱ������Ҫ������
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

    //�������������ݼ��غ���
    //���߳�ûɶ��ϵ
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
