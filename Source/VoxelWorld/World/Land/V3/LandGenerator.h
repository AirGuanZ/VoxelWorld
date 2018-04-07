/*================================================================
Filename: V3/LandGenerator.h
Date: 2018.4.7
Created by AirGuanZ
================================================================*/
#pragma once

#include <random>

#include <World\Land\LandGenerator.h>

namespace LandGenerator_V3
{
    using RandomEngine = std::default_random_engine;
    using Seed = RandomEngine::result_type;

    class LandGenerator : public ::LandGenerator
    {
    public:
        LandGenerator(Seed seed)
            : seed_(seed)
        {

        }

        void GenerateLand(Chunk *ck);

    private:
        Seed seed_;
    };
}
