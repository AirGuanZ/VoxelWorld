/*================================================================
Filename: V3/LandGenerator_V3.h
Date: 2018.4.7
Created by AirGuanZ
================================================================*/
#pragma once

#include <random>

#include <World\Land\LandGenerator.h>

namespace LandGenerator_V3
{
    using RandomEngine = std::mt19937_64;
    using Seed = RandomEngine::result_type;

    class LandGenerator : public ::LandGenerator
    {
    public:
        LandGenerator(Seed seed) { }

        void GenerateLand(Chunk *ck);
    };
}
