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

struct ChunkSectionModels
{
    BasicModel basic[BASIC_RENDERER_TEXTURE_NUM];
};

class Chunk
{
public:

private:
    Block blocks_[CHUNK_SECTION_SIZE][CHUNK_SECTION_SIZE][CHUNK_MAX_HEIGHT];
    BasicModel *models_[CHUNK_SECTION_NUM];
};

#endif //VW_CHUNK_H
