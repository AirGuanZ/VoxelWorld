/*================================================================
Filename: LandGenerator.h
Date: 2018.1.21
Created by AirGuanZ
================================================================*/
#ifndef VW_LAND_GENERATOR_H
#define VW_LAND_GENERATOR_H

#include "../Chunk/Chunk.h"

class LandGenerator
{
public:
    virtual void GenerateLand(Chunk *ck) = 0;
};

#endif //VW_LAND_GENERATOR_H
