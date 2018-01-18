/*================================================================
Filename: ChunkLoader.cpp
Date: 2018.1.16
Created by AirGuanZ
================================================================*/
#include <cassert>
#include <chrono>

#include "ChunkLoader.h"

ChunkLoader::ChunkLoader(void)
{
    running_ = false;
}

ChunkLoader::~ChunkLoader(void)
{
    Destroy();
}

void ChunkLoader::Initialize(int threadsNum)
{
    int nThreads = threadsNum;
    if(nThreads <= 0)
        nThreads = (std::max)(1u, std::thread::hardware_concurrency());
    assert(nThreads > 0);

    running_ = true;
    while(nThreads-- > 0)
        threads_.emplace_back(&ChunkLoader::LoadingThreadEntry, this);
}

void ChunkLoader::Destroy(void)
{
    running_ = false;
    for(std::thread &thread : threads_)
    {
        if(thread.joinable())
            thread.join();
    }
}

void ChunkLoader::AddTask(ChunkLoaderTask *task)
{
    assert(task != nullptr);

    std::lock_guard<std::mutex> lg(taskQueueMutex_);
    tasks_.push(task);
}

void ChunkLoader::LoadingThreadEntry(void)
{
    while(running_)
    {
        ChunkLoaderTask *task = nullptr;

        {
            std::lock_guard<std::mutex> lg(taskQueueMutex_);
            if(!tasks_.empty())
            {
                task = tasks_.front();
                tasks_.pop();
            }
        }

        if(task)
        {
            task->Run();
            delete task;
        }
        else
            std::this_thread::sleep_for(std::chrono::milliseconds(3));
    }
}
