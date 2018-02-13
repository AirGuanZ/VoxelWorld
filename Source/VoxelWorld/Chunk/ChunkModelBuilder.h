/*================================================================
Filename: ChunkModelBuilder.h
Date: 2018.1.18
Created by AirGuanZ
================================================================*/
#ifndef VW_CHUNK_MODEL_BUILDER_H
#define VW_CHUNK_MODEL_BUILDER_H

#include <Utility/Uncopiable.h>

#include "../Block/BlockModelBuilder.h"

class ChunkManager;

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

class BackgroundChunkModelBuilder : public Uncopiable
{
public:
    ChunkSectionModels *Build(Chunk *(&cks)[3][3], int section) const;
};

#endif //VW_CHUNK_MODEL_BUILDER_H
