/*================================================================
Filename: V2/Noise_V2.h
Date: 2018.2.3
Created by AirGuanZ
================================================================*/
#ifndef VW_NOISE_H
#define VW_NOISE_H

#include <cmath>

#include "Common_V2.h"

namespace LandGenerator_V2
{
    class SimpleNoiseInterpolator
    {
    public:
        float operator()(float a, float b, float t)
        {
            return a + (b - a) * (t * t * t * (t * (t * 6 - 15) + 10));
        }
    };

    class NoiseLayer
    {
    public:
        NoiseLayer(Seed seed, float gridSize)
            : seed_(seed), gridSize_(gridSize)
        {

        }

        template<typename LerpFuncType = SimpleNoiseInterpolator>
        float Get(float x, float z, LerpFuncType &&lerp = SimpleNoiseInterpolator()) const
        {
            int gridX = static_cast<int>(std::floor(x / gridSize_));
            int gridZ = static_cast<int>(std::floor(z / gridSize_));

            auto RandXZ = [=](int x, int z) -> float
            {
                return std::uniform_real_distribution<float>(0.0f, 1.0f)
                    (RandomEngine((seed_ + x) * z +
                    std::uniform_int_distribution<Seed>()(RandomEngine(x))));
            };

            float xz   = RandXZ(gridX, gridZ);
            float x1z  = RandXZ(gridX + 1, gridZ);
            float xz1  = RandXZ(gridX, gridZ + 1);
            float x1z1 = RandXZ(gridX + 1, gridZ + 1);
            float tX   = (x - gridX * gridSize_) / gridSize_;
            float tZ   = (z - gridZ * gridSize_) / gridSize_;

            return lerp(lerp(xz, x1z, tX), lerp(xz1, x1z1, tX), tZ);
        }

    private:
        float gridSize_;
        Seed seed_;
    };
}

#endif //VW_NOISE_H
