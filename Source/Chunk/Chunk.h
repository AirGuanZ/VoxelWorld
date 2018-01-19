/*================================================================
Filename: Chunk.h
Date: 2018.1.18
Created by AirGuanZ
================================================================*/
#ifndef VW_CHUNK_H
#define VW_CHUNK_H

#include "../Block/Block.h"
#include "../Model/BasicModel.h"
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

class Chunk
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

private:
    ChunkManager *ckMgr_;
    IntVectorXZ ckPos_;

    //下标的使用：[x][y][z]
    BlockData blocks_;
    ChunkSectionModels *models_[CHUNK_SECTION_NUM];
};

#endif //VW_CHUNK_H
