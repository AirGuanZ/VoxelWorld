/*================================================================
Filename: Common.h
Date: 2018.1.30
Created by AirGuanZ
================================================================*/
#pragma once

#include <random>

namespace LandGenerator_V1
{
    using RandomEngine = std::default_random_engine;
    using Seed = RandomEngine::result_type;
}
