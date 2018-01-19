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

class ChunkLoaderMessage
{
public:
    enum { ChunkLoaded } type;
    union
    {
        Chunk *ckLoaded;
    };
};

class ChunkLoader
{
public:
    ~ChunkLoader(void);

    void Initialize(int threadNum = -1);
    void Destroy(void);

    void AddTask(ChunkLoaderTask *task);
    void AddMsg(ChunkLoaderMessage *msg);

    std::shared_ptr<ChunkLoaderMessage> FetchMsg(void);

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

    void Run(ChunkLoader *loader);

private:
    Chunk *ck_;
};

class ChunkLoaderTask_DestroyChunk : public ChunkLoaderTask
{
public:
    ChunkLoaderTask_DestroyChunk(Chunk *ck);

    void Run(ChunkLoader *loader);

private:
    Chunk *ck_;
};

#endif //VW_CHUNK_LOADER_H
