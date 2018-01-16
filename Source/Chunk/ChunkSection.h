/*================================================================
Filename: ChunkSection.h
Date: 2018.1.15
Created by AirGuanZ
================================================================*/
#ifndef VW_CHUNK_SECTION_H
#define VW_CHUNK_SECTION_H

#include "../Model/BasicModel.h"
#include "../Utility/D3D11Header.h"
#include "../Utility/Uncopiable.h"
#include "Block.h"

constexpr int CHUNKSECTION_SIZE = 16;

/*
    ChunkSection自带一个线程安全的引用计数器
    主线程永远持有一个引用计数，除非打算销毁它
    每个loader任务都持有一个引用计数

    ChunkSection中的数据分为两部分：

    1. Block数据
        这部分数据一开始由loader线程写入，交于主线程后仅由主线程写入
        销毁时，主线程提交销毁任务给loader线程

    2. Model数据
        这部分数据永远由loader线程负责创建和销毁
        如果主线程needNewModel，那么就拷贝一份block + 周围一圈block的数据，
        用拷贝的数据作为创建model的loader任务的数据源
*/

class ChunkSection : public Uncopiable
{
public:
    ChunkSection(void);
    ~ChunkSection(void);

    void SetBlock(int xInSection, int yInSection, int zInSection, const Block &block);
    const Block &GetBlock(int xInSection, int yInSection, int zInSection) const;
    Block &GetBlock(int xInSection, int yInSection, int zInSection);

    bool IsModelsAvailable(void) const;

    void GetBlockData(Block (&data)[CHUNKSECTION_SIZE][CHUNKSECTION_SIZE][CHUNKSECTION_SIZE]) const;
    
    void GetPosXFaceBlockData(Block (&data)[CHUNKSECTION_SIZE][CHUNKSECTION_SIZE]) const;
    void GetNegXFaceBlockData(Block (&data)[CHUNKSECTION_SIZE][CHUNKSECTION_SIZE]) const;

    void GetPosYFaceBlockData(Block (&data)[CHUNKSECTION_SIZE][CHUNKSECTION_SIZE]) const;
    void GetNegYFaceBlockData(Block (&data)[CHUNKSECTION_SIZE][CHUNKSECTION_SIZE]) const;

    void GetPosZFaceBlockData(Block (&data)[CHUNKSECTION_SIZE][CHUNKSECTION_SIZE]) const;
    void GetNegZFaceBlockData(Block (&data)[CHUNKSECTION_SIZE][CHUNKSECTION_SIZE]) const;

private:
    Block blocks_[CHUNKSECTION_SIZE][CHUNKSECTION_SIZE][CHUNKSECTION_SIZE];

    BasicModel *basicModel_;
    bool blockChanged_;
};

#endif //VW_CHUNK_SECTION_H
