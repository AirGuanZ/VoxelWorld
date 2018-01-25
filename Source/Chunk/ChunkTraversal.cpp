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
        for(int z = 1; z < CHUNK_SECTION_SIZE; ++z)
            XZInternal_.push_back({ x, z });
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
