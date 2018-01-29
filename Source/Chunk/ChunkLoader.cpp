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

ChunkLoader::ChunkLoader(size_t ckPoolSize)
    : ckPool_(ckPoolSize)
{

}

ChunkLoader::~ChunkLoader(void)
{
    Destroy();
}

void ChunkLoader::Initialize(int threadNum)
{
    assert(threads_.empty());
    if(threadNum <= 0)
        threadNum = (std::max)(4u, std::thread::hardware_concurrency()) - 2;

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

    ckPool_.Destroy();
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

    std::vector<IntVector3> lightUpdates;
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

namespace
{
    inline bool OutOfBound(int x, int y, int z)
    {
        return (x | y | z | (3 * CHUNK_SECTION_SIZE - 1 - x) |
                            (3 * CHUNK_SECTION_SIZE - 1 - z) |
                            (CHUNK_MAX_HEIGHT - 1 - y)) < 0;
    }

    inline BlockLight GetLight(Chunk *(&cks)[3][3], int x, int y, int z)
    {
        if(OutOfBound(x, y, z))
        {
            return LIGHT_ALL_MAX;
        }
        return cks[x / CHUNK_SECTION_SIZE][z / CHUNK_SECTION_SIZE]
            ->GetBlockLight(x % CHUNK_SECTION_SIZE, y, z % CHUNK_SECTION_SIZE);
    }

    inline int GetHeight(Chunk *(&cks)[3][3], int x, int z)
    {
        if(OutOfBound(x, 0, z))
            return 0;
        return cks[x / CHUNK_SECTION_SIZE][z / CHUNK_SECTION_SIZE]
            ->GetHeight(x % CHUNK_SECTION_SIZE, z % CHUNK_SECTION_SIZE);
    }

    inline BlockType GetType(Chunk *(&cks)[3][3], int x, int y, int z)
    {
        if(OutOfBound(x, y, z))
        {
            return BlockType::Air;
        }
        return cks[x / CHUNK_SECTION_SIZE][z / CHUNK_SECTION_SIZE]
            ->GetBlockType(x % CHUNK_SECTION_SIZE, y, z % CHUNK_SECTION_SIZE);
    }

    inline void SetLight(Chunk *(&cks)[3][3], int x, int y, int z, BlockLight light)
    {
        if(OutOfBound(x, y, z))
            return;
        cks[x / CHUNK_SECTION_SIZE][z / CHUNK_SECTION_SIZE]
            ->SetBlockLight(x % CHUNK_SECTION_SIZE, y, z % CHUNK_SECTION_SIZE, light);
    }

    void LightProg(Chunk *(&cks)[3][3])
    {
        BlockInfoManager &infoMgr = BlockInfoManager::GetInstance();
        std::deque<IntVector3> progQueue;

        auto DirectionalUpdate = [&](BlockLight cenLight, int ax, int ay, int az) -> void
        {
            BlockLight nX = GetLight(cks, ax, ay, az);
            BlockLight newNX = BlockLightMax(nX, BlockLightMinus(
                cenLight, infoMgr.GetBlockInfo(GetType(cks, ax, ay, az)).lightDec));
            if(newNX != nX)
            {
                SetLight(cks, ax, ay, az, newNX);
                if(!OutOfBound(ax, ay, az))
                    progQueue.push_back({ ax, ay, az });
            }
        };

        auto TryAsSource = [&](int x, int y, int z)
        {
            if(!OutOfBound(x, y, z) && GetLight(cks, x, y, z) != LIGHT_ALL_MIN)
            {
                progQueue.push_back({ x, y, z });
                while(progQueue.size())
                {
                    auto[x, y, z] = progQueue.front();
                    progQueue.pop_front();

                    BlockLight cenLight = GetLight(cks, x, y, z);

                    DirectionalUpdate(cenLight, x - 1, y, z);
                    DirectionalUpdate(cenLight, x + 1, y, z);
                    DirectionalUpdate(cenLight, x, y - 1, z);
                    DirectionalUpdate(cenLight, x, y + 1, z);
                    DirectionalUpdate(cenLight, x, y, z - 1);
                    DirectionalUpdate(cenLight, x, y, z + 1);
                }
            }
        };

        for(int x = 0; x < 3 * CHUNK_SECTION_SIZE; ++x)
        {
            for(int z = 0; z < 3 * CHUNK_SECTION_SIZE; ++z)
            {
                int H = GetHeight(cks, x, z);
                for(int y = 0; y <= H; ++y)
                {
                    if(infoMgr.GetBlockInfo(GetType(cks, x, y, z)).lightDec < LIGHT_COMPONENT_MAX)
                    {
                        TryAsSource(x - 1, y, z);
                        TryAsSource(x + 1, y, z);
                        TryAsSource(x, y - 1, z);
                        TryAsSource(x, y + 1, z);
                        TryAsSource(x, y, z - 1);
                        TryAsSource(x, y, z + 1);
                    }
                }
            }
        }
    }
}

void ChunkLoader::LoadChunkData(Chunk *ck)
{
    IntVectorXZ ckPos;
    Chunk *neis = new Chunk[8]
    {
        { ck->GetChunkManager(), { ckPos.x - 1, ckPos.z } },        //0
        { ck->GetChunkManager(), { ckPos.x + 1, ckPos.z } },        //1
        { ck->GetChunkManager(), { ckPos.x, ckPos.z - 1 } },        //2
        { ck->GetChunkManager(), { ckPos.x, ckPos.z + 1 } },        //3
        { ck->GetChunkManager(), { ckPos.x - 1, ckPos.z - 1 } },    //4
        { ck->GetChunkManager(), { ckPos.x - 1, ckPos.z + 1 } },    //5
        { ck->GetChunkManager(), { ckPos.x + 1, ckPos.z - 1 } },    //6
        { ck->GetChunkManager(), { ckPos.x + 1, ckPos.z + 1 } },    //7
    };

    constexpr LandGenerator_V0::Seed seed = 123;

    for(int i = 0; i != 8; ++i)
    {
        Chunk &dst = neis[i];
        if(!ckPool_.GetChunk(dst))
        {
            LandGenerator_V0(seed).GenerateLand(&dst);

            Chunk *addedCk = new Chunk(dst.GetChunkManager(), dst.GetPosition());
            CopyChunkData(*addedCk, dst);
            ckPool_.AddChunk(addedCk);
        }
    }

    if(!ckPool_.GetChunk(*ck))
    {
        LandGenerator_V0(seed).GenerateLand(ck);

        Chunk *addedCk = new Chunk(ck->GetChunkManager(), ck->GetPosition());
        CopyChunkData(*addedCk, *ck);
        ckPool_.AddChunk(addedCk);
    }

    Chunk *cks[3][3] =
    {
        { neis + 4, neis + 0, neis + 5 },
        { neis + 2, ck,       neis + 3 },
        { neis + 6, neis + 1, neis + 7 }
    };
    LightProg(cks);

    delete[] neis;
}
