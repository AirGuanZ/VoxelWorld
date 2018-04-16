/*================================================================
Filename: V1/Palm.h
Date: 2018.2.2
Created by AirGuanZ
================================================================*/
#pragma once

#include "Biome_V1.h"
#include "Common_V1.h"
#include "LandGenerator_V1.h"

namespace LandGenerator_V1
{
    class PalmGenerator
    {
    public:
        PalmGenerator(Seed seed);

        void Make(Chunk *ck, const BiomeGenerator &biome) const;

    private:
        float Random(Seed seedOffset, int blkX, int blkZ, float min, float max) const;

        void Try(Chunk *ck, int blkX, int blkY, int blkZ) const;

    private:
        Seed seed_;
    };
}
