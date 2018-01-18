/*================================================================
Filename: ChunkSection.h
Date: 2018.1.15
Created by AirGuanZ
================================================================*/
#ifndef VW_CHUNK_SECTION_H
#define VW_CHUNK_SECTION_H

#include "../Block/Block.h"
#include "../Model/BasicModel.h"
#include "../Renderer/BasicRenderer.h"
#include "../Utility/D3D11Header.h"
#include "../Utility/Uncopiable.h"

/*
    ChunkSection中的数据分为两部分：

    1. Block数据
        这部分数据一开始由loader线程写入，交于主线程后仅由主线程写入
        Block改变导致的模型重建也在主线程完成
        销毁时，主线程提交销毁任务给loader线程

    2. Model数据
        新Chunk的Model由loader线程负责创建
        Model的销毁永远由loader线程负责
        如果主线程needNewModel，那么就拷贝一份block + 周围一圈block的数据，
        用拷贝的数据作为创建model的loader任务的数据源

    保证：loader任务不依赖于主线程这边任何ChunkSection的数据
*/

constexpr int CHUNK_SECTION_SIZE = 16;

struct ChunkSectionModels
{
    BasicModel basicModel[BASIC_RENDERER_TEXTURE_NUM];
};

class ChunkSection : public Uncopiable
{
public:
    friend class Chunk;

    using ChunkSectionData = Block[CHUNK_SECTION_SIZE][CHUNK_SECTION_SIZE][CHUNK_SECTION_SIZE];

    ChunkSection(void);
    ~ChunkSection(void);

    void SetBlock(int xInSection, int yInSection, int zInSection, const Block &block);
    const Block &GetBlock(int xInSection, int yInSection, int zInSection) const;
    Block &GetBlock(int xInSection, int yInSection, int zInSection);

    bool IsModelsAvailable(void) const;

    const ChunkSectionData &GetBlockData(void) const;

private:
    ChunkSectionData blocks_;

    ChunkSectionModels *models_;
    bool blockChanged_;
};

#endif //VW_CHUNK_SECTION_H
