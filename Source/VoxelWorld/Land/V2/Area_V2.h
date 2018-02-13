/*================================================================
Filename: V2/Area.h
Date: 2018.2.3
Created by AirGuanZ
================================================================*/
#ifndef VW_LANGGEN_V2_AREA_H
#define VW_LANGGEN_V2_AREA_H

#include <Utility/Math.h>

#include "../../Chunk/Chunk.h"
#include "Common_V2.h"

namespace LandGenerator_V2
{
    constexpr int AREA_GRID_CHUNK_SIZE = 64;
    constexpr int AREA_GRID_BLOCK_SIZE = AREA_GRID_CHUNK_SIZE * CHUNK_SECTION_SIZE;
    constexpr int AREA_POSITION_CONVENTION_POSITIVE_OFFSET = 0x40000000;

    inline int ChunkXZ_To_AreaGridXZ(int pos)
    {
        return (pos + AREA_POSITION_CONVENTION_POSITIVE_OFFSET) / AREA_GRID_CHUNK_SIZE
            - AREA_POSITION_CONVENTION_POSITIVE_OFFSET / AREA_GRID_CHUNK_SIZE;
    }

    inline IntVectorXZ ChunkXZ_To_AreaGridXZ(const IntVectorXZ pos)
    {
        return { ChunkXZ_To_AreaGridXZ(pos.x), ChunkXZ_To_AreaGridXZ(pos.z) };
    }

    inline int ChunkXZ_To_ChunkXZInAreaGrid(int pos)
    {
        return (pos + AREA_POSITION_CONVENTION_POSITIVE_OFFSET) % AREA_GRID_CHUNK_SIZE;
    }

    inline IntVectorXZ ChunkXZ_To_ChunkXZInAreaGrid(const IntVectorXZ pos)
    {
        return { ChunkXZ_To_ChunkXZInAreaGrid(pos.x), ChunkXZ_To_ChunkXZInAreaGrid(pos.z) };
    }

    enum class AreaType
    {
        Normal,
        Desert,

        AreaTypeNum = Desert + 1
    };

    class Area
    {
    public:
        struct ResultUnit
        {
            AreaType type = AreaType::Normal;
            float factor  = 1.0f;
        };

        Area(Seed seed);

        void Generate(int ckX, int ckZ);

        const ResultUnit &GetResult(int bkX, int bkZ) const;

    private:
        Seed seed_;

        ResultUnit result_[CHUNK_SECTION_SIZE][CHUNK_SECTION_SIZE];
    };
}

#endif //VW_LANGGEN_V2_AREA_H
