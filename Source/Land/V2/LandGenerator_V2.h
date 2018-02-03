/*================================================================
Filename: V2/LandGenerator.h
Date: 2018.2.3
Created by AirGuanZ
================================================================*/
#ifndef VW_LANDGEN_V2_LAND_GENERATOR_H
#define VW_LANDGEN_V2_LAND_GENERATOR_H

#include <vector>

#include "Common_V2.h"
#include "Noise_V2.h"

class Chunk;

namespace LandGenerator_V2
{
    class LandGenerator
    {
    public:
        LandGenerator(Seed seed);

        void GenerateLand(Chunk *ck) const;

    private:
        Seed seed_;

        std::vector<NoiseLayer> mountLayers_;
        std::vector<NoiseLayer> fieldLayers_;

        std::vector<float> mountHeights_;
        std::vector<float> fieldHeights_;

        float mountHeightSum_;
        float fieldHeightSum_;
    };
}

#endif //VW_LANDGEN_V2_LAND_GENERATOR_H
