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

class TestLandGenerator
{
public:
    static TestLandGenerator instance_;
    void GenerateLand(Chunk *ck);
};

#endif //VW_LAND_GENERATOR_H
