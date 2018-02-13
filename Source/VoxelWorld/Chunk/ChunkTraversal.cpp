/*================================================================
Filename: ChunkTraversal.cpp
Date: 2018.1.25
Created by AirGuanZ
================================================================*/
#include "ChunkTraversal.h"

SINGLETON_CLASS_DEFINITION(ChunkTraversal);

ChunkTraversal::ChunkTraversal(void)
{
    //初始化水平方向的一圈

    XZEdge_.reserve(CHUNK_SECTION_SIZE * CHUNK_SECTION_SIZE -
        (CHUNK_SECTION_SIZE - 2) * (CHUNK_SECTION_SIZE - 2));
    for(int x = 0; x < CHUNK_SECTION_SIZE; ++x)
    {
        XZEdge_.push_back({ x, 0 });
        XZEdge_.push_back({ x, CHUNK_SECTION_SIZE - 1 });
    }
    for(int z = 1; z < CHUNK_SECTION_SIZE - 1; ++z)
    {
        XZEdge_.push_back({ 0, z });
        XZEdge_.push_back({ CHUNK_SECTION_SIZE - 1, z });
    }

    //初始化内部XZ坐标

    XZInternal_.reserve((CHUNK_SECTION_SIZE - 2) * (CHUNK_SECTION_SIZE - 2));
    for(int x = 1; x < CHUNK_SECTION_SIZE - 1; ++x)
    {
        for(int z = 1; z < CHUNK_SECTION_SIZE - 1; ++z)
            XZInternal_.push_back({ x, z });
    }

    //初始化外面一圈
    //先四个侧面，然后顶面和地面

    for(const IntVectorXZ &e : XZEdge_)
    {
        for(int y = 0; y != CHUNK_SECTION_SIZE; ++y)
            XYZFaces_.push_back({ e.x, y, e.z });
    }

    for(const IntVectorXZ &i : XZInternal_)
    {
        XYZFaces_.push_back({ i.x, 0, i.z });
        XYZFaces_.push_back({ i.x, CHUNK_SECTION_SIZE - 1, i.z });
    }
}

const std::vector<IntVectorXZ> &ChunkTraversal::ChunkXZEdge(void) const
{
    return XZEdge_;
}

const std::vector<IntVectorXZ> &ChunkTraversal::ChunkXZInternal(void) const
{
    return XZInternal_;
}

const std::vector<IntVector3> &ChunkTraversal::ChunkFaces(void) const
{
    return XYZFaces_;
}
