/*================================================================
Filename: LandGenerator.h
Date: 2018.4.6
Created by AirGuanZ
================================================================*/
#pragma once

#include <World\Chunk\Chunk.h>

class LandGenerator
{
public:
    virtual void GenerateLand(Chunk *ck) = 0;
    virtual ~LandGenerator(void) { }
};
