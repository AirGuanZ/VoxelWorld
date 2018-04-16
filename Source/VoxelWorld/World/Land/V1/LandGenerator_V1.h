/*================================================================
Filename: V1/LandGenerator.h
Date: 2018.1.30
Created by AirGuanZ
================================================================*/
#pragma once

#include <World\Land\LandGenerator.h>
#include "Biome_V1.h"
#include "Common_V1.h"

namespace LandGenerator_V1
{
    class LandGenerator : public ::LandGenerator
    {
    public:
        LandGenerator(Seed seed)
            : seed_(seed)
        {

        }

        void GenerateLand(Chunk *ck);

    private:
        float Random(Seed seedOffset, int blkX, int blkZ, float min, float max) const;
        float Noise(Seed offset, int x, int z) const;

        float BaseHeight(BiomeType type);
        float VariHeight(BiomeType type);

        void MakeOcean(Chunk *ck, int x, int z, int h);
        void MakeField(Chunk *ck, int x, int z, int h);
        void MakePlain(Chunk *ck, int x, int z, int h);
        void MakeDesert(Chunk *ck, int x, int z, int h);
        void MakeHill(Chunk *ck, int x, int z, int h);

    private:
        Seed seed_;
    };
}
