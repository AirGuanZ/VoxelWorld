/*================================================================
Filename: Common.h
Date: 2018.2.3
Created by AirGuanZ
================================================================*/
#pragma once

#include <random>

namespace LandGenerator_V2
{
    using RandomEngine = std::mt19937_64;
    using Seed = RandomEngine::result_type;
}
