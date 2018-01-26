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
    std::vector<IntVector3> uniLightUpdates; //world block positions
};

class ChunkLoader
{
public:
    ~ChunkLoader(void);

    void Initialize(int threadNum = -1);
    void Destroy(void);

    void AddTask(ChunkLoaderTask *task);
    void AddMsg(ChunkLoaderMessage *msg);

    ChunkLoaderMessage *FetchMsg(void);
    std::queue<ChunkLoaderMessage*> FetchAllMsgs(void);

    //真正的区块数据加载函数
    //和线程没啥关系
    void LoadChunkData(Chunk *ck, std::vector<IntVector3> &lightUpdates);

private:
    void TaskThreadEntry(void);

private:
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

private:
    Chunk *ck_;
};

#endif //VW_CHUNK_LOADER_H
