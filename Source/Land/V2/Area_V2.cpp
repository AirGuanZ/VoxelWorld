/*================================================================
Filename: V2/Area.cpp
Date: 2018.2.3
Created by AirGuanZ
================================================================*/
#include <cmath>
#include <limits>

#include "Area_V2.h"

using namespace LandGenerator_V2;

namespace
{
    inline Seed CombineSeed(Seed s1, Seed s2) noexcept
    {
        std::uniform_int_distribution<Seed> dis(0, (std::numeric_limits<Seed>::max)());
        return dis(RandomEngine(dis(RandomEngine(s1)) + 7 * dis(RandomEngine(s2))));
    }

    inline IntVectorXZ RandCentre(Seed seed, int t, int z)
    {
        RandomEngine eng(CombineSeed(CombineSeed(seed, t), z));
        std::uniform_int_distribution<int> dis(0, AREA_GRID_BLOCK_SIZE - 1);
        IntVectorXZ rt;
        rt.x = dis(eng), rt.z = dis(eng);
        return rt;
    }

    inline AreaType RandType(Seed seed, int t, int z)
    {
        return static_cast<AreaType>(std::uniform_int_distribution<int>(
            0, static_cast<int>(AreaType::AreaTypeNum) - 1)
            (RandomEngine(CombineSeed(CombineSeed(seed + 13, t), z))));
    }
}

Area::Area(Seed seed)
    : seed_(seed)
{

}

void Area::Generate(int ckX, int ckZ)
{
    auto [gridX, gridZ] = ChunkXZ_To_AreaGridXZ({ ckX, ckZ });
    auto [inCkX, inCkZ] = ChunkXZ_To_ChunkXZInAreaGrid({ ckX, ckZ });

    constexpr int GRID_RADIUS = 2;
    constexpr int GRID_NUM = 2 * GRID_RADIUS + 1;
    constexpr int PNT_NUM_IN_GRID = 2;

    struct GridPnt
    {
        IntVectorXZ pos;
        AreaType type;
    };
    std::vector<GridPnt> pnts;

    for(int it = 0; it != GRID_NUM; ++it)
    {
        int gt = gridX + it - GRID_RADIUS;
        for(int iz = 0; iz != GRID_NUM; ++iz)
        {
            int gz = gridZ + iz - GRID_RADIUS;

            for(int i = 0; i != PNT_NUM_IN_GRID; ++i)
            {
                IntVectorXZ pos = AREA_GRID_BLOCK_SIZE *
                    IntVectorXZ({ it - GRID_RADIUS, iz - GRID_RADIUS }) +
                    RandCentre(seed_ * i * 13, gt, gz);
                AreaType type = RandType(seed_, gt, gz);
                pnts.push_back({ pos, type });
            }
        }
    }

    int tBase = CHUNK_SECTION_SIZE * inCkX;
    int zBase = CHUNK_SECTION_SIZE * inCkZ;

    for(int bt = 0; bt != CHUNK_SECTION_SIZE; ++bt)
    {
        int t = tBase + bt;
        for(int bz = 0; bz != CHUNK_SECTION_SIZE; ++bz)
        {
            int z = zBase + bz;

            AreaType minDisType  = AreaType::Normal;
            AreaType minDisType2 = AreaType::Normal;
            float minDis  = (std::numeric_limits<float>::max)();
            float minDis2 = (std::numeric_limits<float>::max)();

            for(const GridPnt &p : pnts)
            {
                float dis = Distance(p.pos, { t, z });
                if(dis < minDis)
                {
                    minDis2 = minDis;
                    minDis = dis;
                    minDisType2 = minDisType;
                    minDisType = p.type;
                }
                else if(dis < minDis2)
                {
                    minDis2 = dis;
                    minDisType2 = p.type;
                }
            }

            float factor = (std::min)(1.0f, std::pow(3000.0f, ((std::min)(1.0f,
                (minDis2 - minDis) / (minDis2 + minDis)) - 0.3f) / 0.7f) / 2.0f);
            result_[bt][bz] = { minDisType, factor };
        }
    }
}

const Area::ResultUnit &Area::GetResult(int t, int z) const
{
    assert(0 <= t && t < CHUNK_SECTION_SIZE);
    assert(0 <= z && z < CHUNK_SECTION_SIZE);
    return result_[t][z];
}
