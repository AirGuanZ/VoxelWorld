/*================================================================
Filename: Chunk.h
Date: 2018.1.18
Created by AirGuanZ
================================================================*/
#ifndef VW_CHUNK_H
#define VW_CHUNK_H

#include "../Block/Block.h"
#include "../Model/BasicModel.h"
#include "../Renderer/RenderQueue.h"
#include "../Utility/Math.h"
#include "../Utility/Uncopiable.h"

constexpr int CHUNK_SECTION_SIZE = 16;
constexpr int CHUNK_SECTION_NUM  = 16;
constexpr int CHUNK_MAX_HEIGHT   = CHUNK_SECTION_SIZE * CHUNK_SECTION_NUM;

class ChunkManager;

struct ChunkSectionModels
{
    BasicModel basic[BASIC_RENDERER_TEXTURE_NUM];
};

struct ChunkSectionRenderQueue
{
    RenderQueue basic[BASIC_RENDERER_TEXTURE_NUM];
};

class Chunk : public Uncopiable
{
public:
    Chunk(ChunkManager *ckMgr, const IntVectorXZ &ckPos);
    ~Chunk(void);

    IntVectorXZ GetPosition(void) const;

    Block &GetBlock(int xBlock, int yBlock, int zBlock);

    void SetBlock(int xBlock, int yBlock, int zBlock, const Block &blk);
    void SetModel(int section, ChunkSectionModels *model);

    using BlockData = Block[CHUNK_SECTION_SIZE][CHUNK_MAX_HEIGHT][CHUNK_SECTION_SIZE];
    BlockData &GetBlockData(void);

    ChunkSectionModels *GetModels(int section);

    void Render(ChunkSectionRenderQueue *renderQueue);

private:
    ChunkManager *ckMgr_;
    IntVectorXZ ckPos_;

    //下标的使用：[x][y][z]
    BlockData blocks_;
    ChunkSectionModels *models_[CHUNK_SECTION_NUM];
};

constexpr int BLOCK_POSITION_CONVENTION_POSITIVE_OFFSET = 0x40000000;

inline int BlockXZ_To_ChunkXZ(int blk)
{
    return (blk + BLOCK_POSITION_CONVENTION_POSITIVE_OFFSET) / CHUNK_SECTION_SIZE
        - BLOCK_POSITION_CONVENTION_POSITIVE_OFFSET / CHUNK_SECTION_SIZE;
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

inline int Camera_To_Block(float cam)
{
    //IMPROVE
    return static_cast<int>(std::floor(cam));
}

#endif //VW_CHUNK_H
