/*================================================================
Filename: BasicCube.cpp
Date: 2018.1.15
Created by AirGuanZ
================================================================*/
#include <cassert>

#include "../Utility/HelperFunctions.h"
#include "BasicCube.h"

BasicCube::~BasicCube(void)
{
    Destroy();
}

bool BasicCube::Initialize(float size, Vector3 posOffset)
{
    assert(!IsAvailable());

    float halfSize = 0.5f * size;
    std::vector<Vertex> vtxData =
    {
        //x+
        { { +halfSize, -halfSize, +halfSize }, { 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
        { { +halfSize, +halfSize, +halfSize }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
        { { +halfSize, +halfSize, -halfSize }, { 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
        { { +halfSize, -halfSize, +halfSize }, { 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
        { { +halfSize, +halfSize, -halfSize }, { 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
        { { +halfSize, -halfSize, -halfSize }, { 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },

        //x-
        { { -halfSize, -halfSize, -halfSize }, { 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
        { { -halfSize, +halfSize, -halfSize }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
        { { -halfSize, +halfSize, +halfSize }, { 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
        { { -halfSize, -halfSize, -halfSize }, { 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
        { { -halfSize, +halfSize, +halfSize }, { 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
        { { -halfSize, -halfSize, +halfSize }, { 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },

        //y+
        { { -halfSize, +halfSize, +halfSize }, { 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
        { { -halfSize, +halfSize, -halfSize }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
        { { +halfSize, +halfSize, -halfSize }, { 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
        { { -halfSize, +halfSize, +halfSize }, { 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
        { { +halfSize, +halfSize, -halfSize }, { 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
        { { +halfSize, +halfSize, +halfSize }, { 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },

        //y-
        { { -halfSize, -halfSize, -halfSize }, { 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
        { { -halfSize, -halfSize, +halfSize }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
        { { +halfSize, -halfSize, +halfSize }, { 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
        { { -halfSize, -halfSize, -halfSize }, { 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
        { { +halfSize, -halfSize, +halfSize }, { 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
        { { +halfSize, -halfSize, -halfSize }, { 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },

        //z-
        { { +halfSize, -halfSize, -halfSize }, { 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
        { { +halfSize, +halfSize, -halfSize }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
        { { -halfSize, +halfSize, -halfSize }, { 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
        { { +halfSize, -halfSize, -halfSize }, { 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
        { { -halfSize, +halfSize, -halfSize }, { 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
        { { -halfSize, -halfSize, -halfSize }, { 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },

        //z+
        { { -halfSize, -halfSize, +halfSize }, { 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
        { { -halfSize, +halfSize, +halfSize }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
        { { +halfSize, +halfSize, +halfSize }, { 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
        { { -halfSize, -halfSize, +halfSize }, { 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
        { { +halfSize, +halfSize, +halfSize }, { 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
        { { +halfSize, -halfSize, +halfSize }, { 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } }
    };

    ID3D11Buffer *buf = Helper::CreateVertexBuffer(vtxData.data(), sizeof(Vertex) * vtxData.size(), false);
    if(!buf)
        return false;

    vtxBufBinding_.startSlot = 0;
    vtxBufBinding_.bufs.push_back(buf);
    vtxBufBinding_.strides.push_back(sizeof(Vertex));
    vtxBufBinding_.offsets.push_back(0);

    return true;
}

void BasicCube::Destroy(void)
{
    if(IsAvailable())
    {
        assert(vtxBufBinding_.bufs.size() == 1 && vtxBufBinding_.bufs[0]);
        vtxBufBinding_.bufs[0]->Release();
        vtxBufBinding_.startSlot = -1;
        vtxBufBinding_.bufs.clear();
        vtxBufBinding_.strides.clear();
        vtxBufBinding_.offsets.clear();
    }
}
