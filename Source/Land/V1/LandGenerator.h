/*================================================================
Filename: V1/LandGenerator.h
Date: 2018.1.30
Created by AirGuanZ
================================================================*/
#ifndef VW_LANDGEN_V1_LAND_GENERATOR_H
#define VW_LANDGEN_V1_LAND_GENERATOR_H

#include "../../Chunk/Chunk.h"
#include "Common.h"

namespace LandGenerator_V1
{
    class LandGenerator
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

#endif //VW_LANDGEN_V1_LAND_GENERATOR_H
