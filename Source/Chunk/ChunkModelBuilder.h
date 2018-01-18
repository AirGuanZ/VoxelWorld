/*================================================================
Filename: ChunkModelBuilder.h
Date: 2018.1.18
Created by AirGuanZ
================================================================*/
#ifndef VW_CHUNK_MODEL_BUILDER_H
#define VW_CHUNK_MODEL_BUILDER_H

#include "../Block/BlockModelBuilder.h"
#include "../Utility/Uncopiable.h"
#include "ChunkManager.h"

class ChunkModelBuilder : public Uncopiable
{
public:
    ChunkModelBuilder(ChunkManager *ckMgr, Chunk *ck, int section);

    ChunkSectionModels *Build(void);

private:
    ChunkManager *ckMgr_;
    Chunk *ck_;
    int section_;
};

#endif //VW_CHUNK_MODEL_BUILDER_H
