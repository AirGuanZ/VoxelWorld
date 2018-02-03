/*================================================================
Filename: Common.h
Date: 2018.2.3
Created by AirGuanZ
================================================================*/
#ifndef VW_LANDGEN_V2_COMMON_H
#define VW_LANDGEN_V2_COMMON_H

#include <random>

namespace LandGenerator_V2
{
    using RandomEngine = std::mt19937_64;
    using Seed = RandomEngine::result_type;
}

#endif //VW_LANDGEN_V2_COMMON_H
