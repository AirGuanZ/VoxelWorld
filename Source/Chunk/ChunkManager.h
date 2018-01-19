/*================================================================
Filename: ChunkManager.h
Date: 2018.1.18
Created by AirGuanZ
================================================================*/
#ifndef VW_CHUNK_MANAGER_H
#define VW_CHUNK_MANAGER_H

#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "../Utility/Math.h"
#include "../Utility/Uncopiable.h"
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
*/

class ChunkManager
{
public:
    ChunkManager(int loadDistance, int renderDistance,
        int maxImpModelUpdates, int maxUniModelUpdates, int maxModelUpdates);
    ~ChunkManager(void);

    //返回的Chunk在本帧内绝不会失效
    Chunk *GetChunk(int ckX, int ckZ);

    Block &GetBlock(int blkX, int blkY, int blkZ);
    void SetBlock(int blkX, int blkY, int blkZ, const Block &blk);

    bool InRenderRange(int ckX, int ckZ);
    bool InLoadingRange(int ckX, int cnZ);

    void SetCentrePosition(int cenCkX, int cenCkZ);

    void MakeSectionModelInvalid(int xSection, int ySection, int zSection);

private:
    //交付一个加载好的Chunk
    void AddChunkData(Chunk *ck);
    //交付一个创建好的Model
    void AddSectionModel(const IntVector3 &pos, ChunkSectionModels *models);
    //立即在主线程加载区块数据
    void LoadChunk(int ckX, int ckZ);

    void ProcessChunkLoaderMessages(void);

    void ProcessModelUpdates(void);

private:
    int loadDistance_;
    int renderDistance_;
    IntVectorXZ centrePos_;

    std::unordered_map<IntVectorXZ, Chunk*, IntVectorXZHasher> chunks_;
    
    std::unordered_set<IntVector3, IntVector3Hasher> importantModelUpdates_;
    std::unordered_set<IntVector3, IntVector3Hasher> unimportantModelUpdates_;

    int maxImpModelUpdates_;
    int maxUniModelUpdates_;
    int maxModelUpdates_;

    ChunkLoader ckLoader_;
};

constexpr int BLOCK_POSITION_CONVENTION_POSITIVE_OFFSET = 0x40000000;

inline int BlockXZ_To_ChunkXZ(int blk)
{
    return (blk + BLOCK_POSITION_CONVENTION_POSITIVE_OFFSET) / CHUNK_SECTION_SIZE
        + BLOCK_POSITION_CONVENTION_POSITIVE_OFFSET / CHUNK_SECTION_SIZE;
}

inline int BlockXZ_To_BlockXZInChunk(int blk)
{
    return (blk + BLOCK_POSITION_CONVENTION_POSITIVE_OFFSET) % CHUNK_SECTION_SIZE;
}

inline int BlockY_To_ChunkSectionIndex(int blk)
{
    return blk / CHUNK_SECTION_SIZE;
}

inline int BlockY_To_BlockYInChunkSection(int blk)
{
    return blk % CHUNK_SECTION_SIZE;
}

inline int ChunkXZ_To_BlockXZ(int ck)
{
    return ck * CHUNK_SECTION_SIZE;
}

inline int ChunkSectionIndex_To_BlockY(int cks)
{
    return cks * CHUNK_SECTION_SIZE;
}

#endif //VW_CHUNK_MANAGER_H
