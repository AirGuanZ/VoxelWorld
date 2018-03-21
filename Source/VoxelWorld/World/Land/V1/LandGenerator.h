/*================================================================
Filename: V1/LandGenerator.h
Date: 2018.1.30
Created by AirGuanZ
================================================================*/
#pragma once

#include "../../Chunk/Chunk.h"
#include "Biome.h"
#include "Common.h"

namespace LandGenerator_V1
{
    class LandGenerator
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

        float BaseHeight(BiomeType type) const;
        float VariHeight(BiomeType type) const;

        void MakeOcean(Chunk *ck, int x, int z, int h) const;
        void MakeField(Chunk *ck, int x, int z, int h) const;
        void MakePlain(Chunk *ck, int x, int z, int h) const;
        void MakeDesert(Chunk *ck, int x, int z, int h) const;
        void MakeHill(Chunk *ck, int x, int z, int h) const;

    private:
        Seed seed_;
    };
}
