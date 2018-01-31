/*================================================================
Filename: V1/Biome.cpp
Date: 2018.1.30
Created by AirGuanZ
================================================================*/
#include <cassert>
#include <limits>

#include "Biome.h"

using namespace LandGenerator_V1;

/*
����άŵͼʵ�֣�ÿ�������������������һ���㣨�����ĵ��Ⱥϵ���ĵ㣩�Լ��õ�ռ�ݵ�Ⱥϵ����

һ�������Ⱥϵ������������������ĵ��Ⱥϵ���ͣ����ھ�����������ĵ�͵ڶ��������ĵ�
�����ϵ�λ�þ�����factor��neiFactor

����Ⱥϵ��Ե�Ĳ������ԣ����ھ���άŵͼ�еĶ���ΰ�
�Ժ�����ö�ά�����������Ϊ�����ж�λ�õ�ƫ����ʵ��
*/

/*
����������뾶ΪA
��ĳ�����������еķ��飬����������������ĵ�ľ��벻����sqrt(2) * A��
�ݴˣ���һ��������������ĵ���ܳ������������񸽽�5 * 5��ȥ�ĸ��ǵķ�Χ�ڣ�
��һ��ϢӦ������������
*/

#define RESULT(X, Z) result[Chunk::XZ((X), (Z))]

namespace
{
    inline Seed CombineSeed(Seed s1, Seed s2) noexcept
    {
        std::uniform_int_distribution<Seed> dis(0, (std::numeric_limits<Seed>::max)());
        return dis(RandomEngine(dis(RandomEngine(s1)) + 7 * dis(RandomEngine(s2))));
    }


    inline IntVectorXZ RandCentre(Seed seed, int x, int z)
    {
        RandomEngine eng(CombineSeed(CombineSeed(seed, x), z));
        std::uniform_int_distribution<int> dis(0, CHUNK_SECTION_SIZE * BIOME_GRID_SIZE - 1);
        IntVectorXZ rt;
        rt.x = dis(eng), rt.z = dis(eng);
        return rt;
    }

    inline BiomeType RandBiome(Seed seed, int x, int z)
    {
        return static_cast<BiomeType>(std::uniform_int_distribution<int>(
            0, static_cast<int>(BiomeType::BiomeTypeNum) - 1)
            (RandomEngine(CombineSeed(CombineSeed(seed + 13, x), z))));
    }
}

BiomeGenerator::BiomeGenerator(Seed seed)
    : seed_(seed)
{
    for(BiomeResult &unit : result)
        unit = BiomeResult{ BiomeType::Field, BiomeType::Field, 1.0f, 0.0f };
}

void BiomeGenerator::Generate(int ckX, int ckZ)
{
    //ȡ����������grid����
    auto [gridX, gridZ] = ChunkXZ_To_BiomeGridXZ({ ckX, ckZ });
    auto [rckX, rckZ]   = ChunkXZ_To_ChunkXZInBiomeGrid({ ckX, ckZ });

    constexpr int GRID_RADIUS = 2;
    constexpr int GRID_NUM = 2 * GRID_RADIUS + 1;
    constexpr int PNT_NUM_IN_GRID = 2;

    struct GridPnt
    {
        IntVectorXZ centre;
        BiomeType type;
    };
    std::vector<GridPnt> pnts;

    for(int ix = 0; ix != GRID_NUM; ++ix)
    {
        int gx = gridX + ix - GRID_RADIUS;
        for(int iz = 0; iz != GRID_NUM; ++iz)
        {
            int gz = gridZ + iz - GRID_RADIUS;

            for(int i = 0; i != PNT_NUM_IN_GRID; ++i)
            {
                IntVectorXZ pos = CHUNK_SECTION_SIZE * BIOME_GRID_SIZE *
                                    IntVectorXZ{ ix - GRID_RADIUS, iz - GRID_RADIUS }
                                    + RandCentre(seed_ * (i * 13) + gx, gx, gz);
                BiomeType type = RandBiome(seed_, gx, gz);
                pnts.push_back({ pos, type });
            }
        }
    }

    int xBase = CHUNK_SECTION_SIZE * rckX;
    int zBase = CHUNK_SECTION_SIZE * rckZ;

    for(int bx = 0; bx != CHUNK_SECTION_SIZE; ++bx)
    {
        int x = xBase + bx;
        for(int bz = 0; bz != CHUNK_SECTION_SIZE; ++bz)
        {
            int z = zBase + bz;

            BiomeType minDisBiome = BiomeType::Field, minDis2Biome = BiomeType::Field;
            float minDis  = (std::numeric_limits<float>::max)();
            float minDis2 = (std::numeric_limits<float>::max)();

            for(const GridPnt &p : pnts)
            {
                float dis = Distance(p.centre, { x, z });
                if(dis < minDis)
                {
                    minDis2 = minDis, minDis = dis;
                    minDis2Biome = minDisBiome, minDisBiome = p.type;
                }
            }

            float factor = (std::min)(0.5f, 0.8f * (minDis2 - minDis) / (minDis2 + minDis)) + 0.5f;
            RESULT(bx, bz) = BiomeResult{ minDisBiome, minDis2Biome, factor, 1.0f - factor };
        }
    }
}

const BiomeGenerator::BiomeResult &BiomeGenerator::GetResult(int x, int z) const
{
    assert(0 <= x && x < CHUNK_SECTION_SIZE);
    assert(0 <= z && z < CHUNK_SECTION_SIZE);
    return RESULT(x, z);
}
