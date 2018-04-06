/*================================================================
Filename: OakGenerator_V0.h
Date: 2018.1.26
Created by AirGuanZ
================================================================*/
#pragma once

#include <World\Chunk\Chunk.h>
#include <World\Chunk\ChunkLoader.h>
#include "LandGenerator_V0.h"

namespace LandGenerator_V0
{
    class OakGenerator
    {
    public:
        OakGenerator(Seed seed);

        void Make(Chunk *ck) const;

    private:
        float Random(Seed seedOffset, int blkX, int blkZ, float min, float max) const;

        void Try(Chunk *ck, int blkX, int blkY, int blkZ) const;

    private:
        Seed seed_;
    };
}
