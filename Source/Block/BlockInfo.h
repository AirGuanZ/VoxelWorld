/*================================================================
Filename: BlockInfo.h
Date: 2018.1.17
Created by AirGuanZ
================================================================*/
#ifndef VW_BLOCK_INFO_H
#define VW_BLOCK_INFO_H

#include <cstdint>
#include <vector>

#include "../Collision/AABB.h"
#include "../Utility/Math.h"

enum class BlockType : std::uint8_t
{
    Air,
    Bedrock,
    Stone,
    Dirt,
    GrassBox,
    Sand,
    Wood,
    Leaf,
    RedGlowStone,
    GreenGlowStone,
    BlueGlowStone,
    Grass,
    Flower,
    Water,

    BlockTypeNum = Water + 1
};

enum class BlockRenderer : std::uint8_t
{
    Null = 0,
    BasicRenderer = Null + 1,
    CarveRenderer = BasicRenderer + 1,
    TransLiquid   = CarveRenderer + 1,
};

enum class BlockShape : std::uint8_t
{
    Null            = 0,
    Box             = Null + 1,
    Cross           = Box + 1,
    Liquid          = Cross + 1,
};

template<typename Dst = int>
inline constexpr Dst Blk2Int(BlockType blockType)
{
    return static_cast<Dst>(blockType);
}

inline constexpr BlockType Int2Blk(int intValue)
{
    return static_cast<BlockType>(intValue);
}

struct BlockInfo
{
    BlockType type;
    std::string name;

    BlockShape shape;
    BlockRenderer renderer;

    bool isSolid;
    bool isRenderable;

    //放置方块的时候能否覆盖
    //典型地，空气和水是true，石头是false
    bool isCoverable;

    bool isGlow;

    int lightDec;
    IntVector3 lightEmission;

    AABB aabb;

    union
    {
        //BasicRenderer, Box的六个面的tex编号以及其上的坐标
        //0：tex编号
        //1-6：+x, -x, +y, -y, +z, -z
        int basicBoxTexPos[7];

        int carveBoxTexPos[7];

        //CarveRenderer, Cross的两个面的tex编号以及其上的坐标
        //0：tex编号
        //1-2：两个面位置坐标
        int carveCrossTexPos[3];

        //LiquidRenderer，Liquid的六个面的tex编号以及其上的坐标
        int transLiquidTexPos[7];
    };
};

#endif //VW_BLOCK_INFO_H
