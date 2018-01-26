/*================================================================
Filename: OakGenerator_V0.h
Date: 2018.1.26
Created by AirGuanZ
================================================================*/
#ifndef VW_OAK_GENERATOR_V0_H
#define VW_OAK_GENERATOR_V0_H

#include "../Chunk/Chunk.h"
#include "../Chunk/ChunkLoader.h"
#include "LandGenerator_V0.h"

class OakGenerator_V0
{
public:
    using Seed = typename LandGenerator_V0::Seed;

    struct Unit
    {
        IntVector3 pos;
        BlockType type;
    };

    OakGenerator_V0(Seed seed);

    void Make(Chunk *ck, std::vector<IntVector3> &lightUpdates) const;
    
private:
    Seed seed_;
};

#endif //VW_OAK_GENERATOR_V0_H
