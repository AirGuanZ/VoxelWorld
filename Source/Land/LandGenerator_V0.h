/*================================================================
Filename: LandGenerator_V0.h
Date: 2018.1.21
Created by AirGuanZ
================================================================*/
#ifndef VW_LAND_GENERATOR_V0_H
#define VW_LAND_GENERATOR_V0_H

#include <random>

#include "LandGenerator.h"

class LandGenerator_V0 : public LandGenerator
{
public:
    using RandomEngine = std::default_random_engine;
    using Seed = RandomEngine::result_type;
    
    LandGenerator_V0(Seed seed);

    void GenerateLand(Chunk *ck);

private:
    int GetHeight(int blkX, int blkZ);

private:
    Seed seed_;
};

#endif //VW_LAND_GENERATOR_V0_H
