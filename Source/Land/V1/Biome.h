/*================================================================
Filename: V1/Biome.h
Date: 2018.1.30
Created by AirGuanZ
================================================================*/
#ifndef VW_LANDGEN_V1_BIOME_H
#define VW_LANDGEN_V1_BIOME_H

#include "../../Chunk/Chunk.h"
#include "Common.h"

namespace LandGenerator_V1
{
    enum class BiomeType
    {
        Ocean,
        Field,
        Plain,
        Desert,
        Hill,

        BiomeTypeNum = Hill + 1,
    };

    constexpr int BIOME_GRID_SIZE = 32;

    inline int ChunkXZ_To_BiomeGridXZ(int pos)
    {
        return (pos + BLOCK_POSITION_CONVENTION_POSITIVE_OFFSET) / BIOME_GRID_SIZE
            - BLOCK_POSITION_CONVENTION_POSITIVE_OFFSET / BIOME_GRID_SIZE;
    }

    inline IntVectorXZ ChunkXZ_To_BiomeGridXZ(const IntVectorXZ pos)
    {
        return { ChunkXZ_To_BiomeGridXZ(pos.x), ChunkXZ_To_BiomeGridXZ(pos.z) };
    }

    inline int ChunkXZ_To_ChunkXZInBiomeGrid(int pos)
    {
        return (pos + BLOCK_POSITION_CONVENTION_POSITIVE_OFFSET) % BIOME_GRID_SIZE;
    }

    inline IntVectorXZ ChunkXZ_To_ChunkXZInBiomeGrid(const IntVectorXZ pos)
    {
        return { ChunkXZ_To_ChunkXZInBiomeGrid(pos.x), ChunkXZ_To_ChunkXZInBiomeGrid(pos.z) };
    }

    class BiomeGenerator
    {
    public:
        //对每个水平方块坐标，生成的群系信息由两部分构成
        //分别是本群系类型和离该点最近的另一个群系的类型
        //factor和neiFactor之和应为1，用来做群系之间的高度插值
        struct BiomeResult
        {
            BiomeType type;
            BiomeType neiType;
            float factor;
        };

        BiomeGenerator(Seed seed);

        void Generate(int ckX, int ckZ);

        const BiomeResult &GetResult(int x, int z) const;

    private:
        Seed seed_;
        BiomeResult result[CHUNK_SECTION_SIZE][CHUNK_SECTION_SIZE];
    };
}

#endif //VW_LANDGEN_V1_BIOME_H
