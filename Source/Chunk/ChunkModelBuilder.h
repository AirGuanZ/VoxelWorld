/*================================================================
Filename: ChunkModelBuilder.h
Date: 2018.1.18
Created by AirGuanZ
================================================================*/
#ifndef VW_CHUNK_MODEL_BUILDER_H
#define VW_CHUNK_MODEL_BUILDER_H

#include "../Utility/Uncopiable.h"
#include "ChunkManager.h"
#include "ChunkSection.h"

class ChunkModelBuilder : public Uncopiable
{
public:
    ChunkModelBuilder(ChunkManager *ckMgr, ChunkSection *section);

    void Build(void);

private:
    ChunkManager *ckMgr_;
    ChunkSection *section_;
};

#endif //VW_CHUNK_MODEL_BUILDER_H
