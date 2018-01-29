/*================================================================
Filename: LandGenerator_V0.h
Date: 2018.1.21
Created by AirGuanZ
================================================================*/
#ifndef VW_LAND_GENERATOR_V0_H
#define VW_LAND_GENERATOR_V0_H

#include <random>
#include <vector>

#include "../Chunk/Chunk.h"

class LandGenerator_V0
{
public:
    using RandomEngine = std::mt19937;
    using Seed = RandomEngine::result_type;
    
    LandGenerator_V0(Seed seed);

    void GenerateLand(Chunk *ck);

private:
    int GetHeight(int blkX, int blkZ);
    float Random(Seed seedOffset, int blkX, int blkZ, float min, float max);

private:
    Seed seed_;
};

#endif //VW_LAND_GENERATOR_V0_H
