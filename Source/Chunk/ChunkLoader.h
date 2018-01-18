/*================================================================
Filename: ChunkLoader.h
Date: 2018.1.16
Created by AirGuanZ
================================================================*/
#ifndef VW_CHUNK_LOADER_H
#define VW_CHUNK_LOADER_H

#include <atomic>
#include <queue>
#include <mutex>
#include <thread>
#include <vector>

#include "../Utility/Uncopiable.h"

class ChunkLoaderTask : public Uncopiable
{
public:
    virtual bool Run(void) = 0;
};

class ChunkLoader : public Uncopiable
{
public:
    ChunkLoader(void);
    ~ChunkLoader(void);

    void Initialize(int threadsNum = -1);
    void Destroy(void);

    void AddTask(ChunkLoaderTask *task);

private:
    void LoadingThreadEntry(void);

    std::mutex taskQueueMutex_;
    std::queue<ChunkLoaderTask*> tasks_;

    std::atomic<bool> running_;
    std::vector<std::thread> threads_;
};

#endif //VW_CHUNK_LOADER_H
