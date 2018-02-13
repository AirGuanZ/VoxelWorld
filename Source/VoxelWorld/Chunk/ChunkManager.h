/*================================================================
Filename: ChunkManager.h
Date: 2018.1.18
Created by AirGuanZ
================================================================*/
#ifndef VW_CHUNK_MANAGER_H
#define VW_CHUNK_MANAGER_H

#include <map>
#include <mutex>
#include <deque>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <Utility/Math.h>
#include <Utility/Uncopiable.h>

#include "../Block/BlockInfoManager.h"
#include "../Camera/Camera.h"
#include "Chunk.h"
#include "ChunkLoader.h"

/*
    Chunk数据加载及模型创建
        主线程维护一个Position -> Chunk的映射M，后台只负责Chunk的数据加载，不负责模型创建
        
        setCentrePos
            当一个Chunk因超出加载范围而被卸载时，清空其剩下的模型创建任务
            当一个新的位置进入加载范围（loadDistance）时，主线程创建加载任务，由后台完成加载
            当一个新的位置进入渲染范围（renderDistance）时，主线程创建对应已有Chunk的模型任务
        implemented in SetCentrePosition
        
        后台加载完成时，提交“加载完成”的消息给主线程，尝试把它加入M
        如果已经有了（加入失败），就再创建一个数据销毁任务，让后台销毁它
        当一个Chunk被成功加入M时，如果它在渲染范围内，创建对应的模型任务
        implemented in AddChunkData & ProcessChunkLoaderMessages

        当创建一个模型任务时，如果有旧模型，留着。当模型任务完成时，替换掉旧模型
        implemented in AddSectionModel

        模型任务分为两类，一类是“重要任务”，一类是“不重要任务”
        重要任务在每一帧尽量多地处理，其阈值较高，且优先级更高
        不重要任务阈值较低，且优先级更低
        每帧处理的重要任务和不重要任务数量之和不超过一个阈值
        implemented in ProcessModelUpdates
*/

class ChunkManager
{
public:
    ChunkManager(int loadDistance, int renderDistance, int unloadDistance);
    ~ChunkManager(void);

    void StartLoading(int loaderCount);
    void Destroy(void);

    //返回的Chunk在本帧内绝不会失效
    Chunk *GetChunk(int ckX, int ckZ)
    {
        auto it = chunks_.find({ ckX, ckZ });
        if(it != chunks_.end())
            return it->second;

        LoadChunk(ckX, ckZ);

        assert((chunks_[{ ckX, ckZ }] != nullptr));
        return chunks_[{ ckX, ckZ }];
    }

    BlockType GetBlockType(int blkX, int blkY, int blkZ)
    {
        if(blkY < 0 || blkY >= CHUNK_MAX_HEIGHT)
        {
            return BlockType::Air;
        }
        return GetChunk(BlockXZ_To_ChunkXZ(blkX), BlockXZ_To_ChunkXZ(blkZ))
            ->GetBlockType(
                BlockXZ_To_BlockXZInChunk(blkX),
                blkY,
                BlockXZ_To_BlockXZInChunk(blkZ));
    }

    BlockLight GetBlockLight(int blkX, int blkY, int blkZ)
    {
        if(blkY < 0 || blkY >= CHUNK_MAX_HEIGHT)
            return LIGHT_ALL_MAX;
        return GetChunk(BlockXZ_To_ChunkXZ(blkX), BlockXZ_To_ChunkXZ(blkZ))
            ->GetBlockLight(
                BlockXZ_To_BlockXZInChunk(blkX),
                blkY,
                BlockXZ_To_BlockXZInChunk(blkZ));
    }

    Block GetBlock(int blkX, int blkY, int blkZ)
    {
        if(blkY < 0 || blkY >= CHUNK_MAX_HEIGHT)
        {
            return { BlockType::Air, LIGHT_ALL_MAX };
        }

        IntVectorXZ ck = BlockXZ_To_ChunkXZ({ blkX, blkZ });
        IntVectorXZ cb = BlockXZ_To_BlockXZInChunk({ blkX, blkZ });
        return GetChunk(ck.x, ck.z)->GetBlock(cb.x, blkY, cb.z);
    }

    void SetBlockType(int blkX, int blkY, int blkZ, BlockType type)
    {
        if(blkY < 0 || blkY >= CHUNK_MAX_HEIGHT)
            return;

        Chunk *ck = GetChunk(BlockXZ_To_ChunkXZ(blkX), BlockXZ_To_ChunkXZ(blkZ));

        int cx = BlockXZ_To_BlockXZInChunk(blkX);
        int cz = BlockXZ_To_BlockXZInChunk(blkZ);

        ck->SetBlockType(cx, blkY, cz, type);

        if(blkY >= ck->heightMap[Chunk::XZ(cx, cz)])
        {
            int newH = CHUNK_MAX_HEIGHT - 1;
            while(newH > 0 && ck->GetBlockType(cx, newH, cz) == BlockType::Air)
                --newH;

            if(newH != ck->GetHeight(cx, cz))
            {
                int L, H;
                std::tie(L, H) = std::minmax(newH, ck->heightMap[Chunk::XZ(cx, cz)]);

                while(H >= L)
                    UpdateLight(blkX, H--, blkZ);

                ck->heightMap[Chunk::XZ(cx, cz)] = newH;
            }
        }

        if(BlockInfoManager::GetInstance().IsGlow(type))
            RemoveLightSource(blkX, blkY, blkZ);
        else
            UpdateLight(blkX, blkY, blkZ);
    }

    void RemoveLightSource(int x, int y, int z);

    void UpdateLight(int x, int y, int z);

    //以给定的射线和已有的方块求交
    //返回true当且仅当在maxLen内找到了满足PickBlockFunc的方块
    //若返回值为true，则
    //      blk存放第一个满足条件的方块
    //      face指出是从哪一面进入该方块的
    //      rtPos指出该方块在Block坐标系中的位置
    //若原点就在满足条件的方块内部，则face值为六个值中的任一个
    //注意：位置越界的dummyBlock也会参与这一求交过程
    using PickBlockFunc = bool(*)(const Block&);
    bool PickBlock(const Vector3 &origin, const Vector3 &dir,
                   float maxLen, float step, PickBlockFunc func,
                   Block &blk, BlockFace &face, IntVector3 &rtPos);

    bool InRenderRange(int ckX, int ckZ)
    {
        return std::abs(ckX - centrePos_.x) <= renderDistance_ &&
            std::abs(ckZ - centrePos_.z) <= renderDistance_;
    }

    bool InLoadingRange(int ckX, int ckZ)
    {
        return centrePos_.x - loadDistance_ <= ckX && ckX <= centrePos_.x + loadDistance_ &&
            centrePos_.z - loadDistance_ <= ckZ && ckZ <= centrePos_.z + loadDistance_;
    }

    bool InUnloadingRange(int ckX, int ckZ)
    {
        return centrePos_.x - unloadDistance_ <= ckX && ckX <= centrePos_.x + unloadDistance_ &&
            centrePos_.z - unloadDistance_ <= ckZ && ckZ <= centrePos_.z + unloadDistance_;
    }

    void SetCentrePosition(int cenCkX, int cenCkZ);

    void MakeSectionModelInvalid(int xSection, int ySection, int zSection);

    void ProcessChunkLoaderMessages(void);

    void ProcessModelUpdates(void);

    void Render(const Camera &cam, ChunkSectionRenderQueue *renderQueue);

    bool DetectCollision(const Vector3 &pnt);
    bool DetectCollision(const AABB &aabb);

private:
    //交付一个加载好的Chunk
    void AddChunkData(Chunk *ck);
    //交付一个创建好的Model
    void AddSectionModel(const IntVector3 &pos, ChunkSectionModels *models);
    //立即在主线程加载区块数据
    void LoadChunk(int ckX, int ckZ);

    void ComputeModelUpdates(int x, int y, int z, std::unordered_set<IntVector3, IntVector3Hasher> &updates);

private:
    int loadDistance_;
    int renderDistance_;
    int unloadDistance_;
    IntVectorXZ centrePos_;

    std::unordered_map<IntVectorXZ, Chunk*, IntVectorXZHasher> chunks_;
    
    std::unordered_set<IntVector3, IntVector3Hasher> modelUpdates_;

    ChunkLoader ckLoader_;
};

#endif //VW_CHUNK_MANAGER_H
