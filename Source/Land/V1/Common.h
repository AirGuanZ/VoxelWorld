/*================================================================
Filename: Common.h
Date: 2018.1.30
Created by AirGuanZ
================================================================*/
#ifndef VW_LANDGEN_V1_COMMON_H
#define VW_LANDGEN_V1_COMMON_H

#include <random>

namespace LandGenerator_V1
{
    using RandomEngine = std::default_random_engine;
    using Seed = RandomEngine::result_type;
}

#endif //VW_LANDGEN_V1_COMMON_H
