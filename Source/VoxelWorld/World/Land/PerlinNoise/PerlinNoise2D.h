/*================================================================
Filename: PerlinNoise2D.h
Date: 2018.1.31
Created by AirGuanZ
================================================================*/
#pragma once

#include <cmath>
#include <random>

template<typename RT>
class DefaultLevelHeight
{
public:
    RT operator()(int level)
    {
        return RT(std::pow(RT(2), level));
    }
};

template<typename RT>
class DefaultGridSize
{
public:
    RT operator()(int level)
    {
        return RT(std::pow(RT(2), level + 2));
    }
};

template<typename RT>
class DefaultPNLerp
{
public:
    RT operator()(RT a, RT b, RT t)
    {
        //return RT(a + (b - a) * (3 * t * t - 2 * t * t * t));
        return RT(a + (b - a) * (t * t * t * (t * (t * 6 - 15) + 10)));
    }
};

template<typename Seed>
class DefaultPNRandComb
{
public:
    Seed operator()(Seed s1, Seed s2, Seed s3, Seed s4)
    {
        return Seed((s1 + s2) * s3 + s2 + s4 * s4);
    }
};

template<typename RandomEngineType  = std::default_random_engine,
         typename GenResultType     = float,
         typename LevelHType        = DefaultLevelHeight<GenResultType>,
         typename GridSType         = DefaultGridSize<GenResultType>,
         typename LerpType          = DefaultPNLerp<GenResultType>,
         typename RandCombType      = DefaultPNRandComb<typename RandomEngineType::result_type>>
class BasicPerlinNoise2D
{
public:
    using RandomEngine = RandomEngineType;
    using RandomCombinator = RandCombType;
    
    using LevelHeight = LevelHType;
    using Lerp = LerpType;
    using GridSize = GridSType;

    using ResultType = GenResultType;
    using Seed = typename RandomEngine::result_type;

    static ResultType Gen(Seed seed, int x, int z, int levelNum, ResultType minV, ResultType maxV)
    {
        using RT = ResultType;
        std::uniform_real_distribution<ResultType> dis(RT(0.0), RT(1.0));
        
        auto RandXZ = [&](int x, int z, int level)
        {
            return dis(RandomEngine(RandomCombinator()(seed, x, z, level)));
        };
        
        RT rt = RT(0.0), base = RT(0.0);
        for(int level = levelNum; level > 0; --level)
        {
            RT gridSize = GridSize()(level);
            
            int gridX = static_cast<int>(std::floor(x / gridSize));
            int gridZ = static_cast<int>(std::floor(z / gridSize));
            
            RT xzH   = RandXZ(gridX, gridZ, level);
            RT x1zH  = RandXZ(gridX + 1, gridZ, level);
            RT xz1H  = RandXZ(gridX, gridZ + 1, level);
            RT x1z1H = RandXZ(gridX + 1, gridZ + 1, level);
            RT tX = RT(x - gridX * gridSize) / gridSize;
            RT tZ = RT(z - gridZ * gridSize) / gridSize;
            
            RT levelHeight = LevelHeight()(level);
            rt   += levelHeight * Lerp()(Lerp()(xzH, x1zH, tX),
                                         Lerp()(xz1H, x1z1H, tX),
                                         tZ);
            base += levelHeight;
        }
        
        return rt / base * (maxV - minV) + minV;
    }
};
