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
        { posOffset + Vector3{ +halfSize, -halfSize, +halfSize }, { 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
        { posOffset + Vector3{ +halfSize, +halfSize, +halfSize }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
        { posOffset + Vector3{ +halfSize, +halfSize, -halfSize }, { 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
        { posOffset + Vector3{ +halfSize, -halfSize, +halfSize }, { 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
        { posOffset + Vector3{ +halfSize, +halfSize, -halfSize }, { 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
        { posOffset + Vector3{ +halfSize, -halfSize, -halfSize }, { 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },

        //x-
        { posOffset + Vector3{ -halfSize, -halfSize, -halfSize }, { 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
        { posOffset + Vector3{ -halfSize, +halfSize, -halfSize }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
        { posOffset + Vector3{ -halfSize, +halfSize, +halfSize }, { 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
        { posOffset + Vector3{ -halfSize, -halfSize, -halfSize }, { 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
        { posOffset + Vector3{ -halfSize, +halfSize, +halfSize }, { 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
        { posOffset + Vector3{ -halfSize, -halfSize, +halfSize }, { 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },

        //y+
        { posOffset + Vector3{ -halfSize, +halfSize, +halfSize }, { 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
        { posOffset + Vector3{ -halfSize, +halfSize, -halfSize }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
        { posOffset + Vector3{ +halfSize, +halfSize, -halfSize }, { 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
        { posOffset + Vector3{ -halfSize, +halfSize, +halfSize }, { 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
        { posOffset + Vector3{ +halfSize, +halfSize, -halfSize }, { 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
        { posOffset + Vector3{ +halfSize, +halfSize, +halfSize }, { 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },

        //y-
        { posOffset + Vector3{ -halfSize, -halfSize, -halfSize }, { 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
        { posOffset + Vector3{ -halfSize, -halfSize, +halfSize }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
        { posOffset + Vector3{ +halfSize, -halfSize, +halfSize }, { 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
        { posOffset + Vector3{ -halfSize, -halfSize, -halfSize }, { 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
        { posOffset + Vector3{ +halfSize, -halfSize, +halfSize }, { 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
        { posOffset + Vector3{ +halfSize, -halfSize, -halfSize }, { 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },

        //z-
        { posOffset + Vector3{ +halfSize, -halfSize, -halfSize }, { 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
        { posOffset + Vector3{ +halfSize, +halfSize, -halfSize }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
        { posOffset + Vector3{ -halfSize, +halfSize, -halfSize }, { 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
        { posOffset + Vector3{ +halfSize, -halfSize, -halfSize }, { 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
        { posOffset + Vector3{ -halfSize, +halfSize, -halfSize }, { 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
        { posOffset + Vector3{ -halfSize, -halfSize, -halfSize }, { 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },

        //z+
        { posOffset + Vector3{ -halfSize, -halfSize, +halfSize }, { 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
        { posOffset + Vector3{ -halfSize, +halfSize, +halfSize }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
        { posOffset + Vector3{ +halfSize, +halfSize, +halfSize }, { 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
        { posOffset + Vector3{ -halfSize, -halfSize, +halfSize }, { 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
        { posOffset + Vector3{ +halfSize, +halfSize, +halfSize }, { 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
        { posOffset + Vector3{ +halfSize, -halfSize, +halfSize }, { 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } }
    };

    std::vector<UINT16> idices =
    {
        0, 1, 2, 3, 4, 5,
        6, 7, 8, 9, 10, 11,
        12, 13, 14, 15, 16, 17,
        18, 19, 20, 21, 22, 23,
        24, 25, 26, 27, 28, 29,
        30, 31, 32, 33, 34, 35
    };

    ID3D11Buffer *buf = Helper::CreateVertexBuffer(vtxData.data(), sizeof(Vertex) * vtxData.size(), false);
    if(!buf)
        return false;

    ID3D11Buffer *idxBuf = Helper::CreateIndexBuffer(idices.data(), sizeof(UINT16) * idices.size(), false);
    if(!idxBuf)
    {
        Helper::ReleaseCOMObjects(buf);
        return false;
    }

    vtxBufBinding_.startSlot = 0;
    vtxBufBinding_.bufs.push_back(buf);
    vtxBufBinding_.indicesFormat = DXGI_FORMAT_R16_UINT;
    vtxBufBinding_.indices = idxBuf;
    vtxBufBinding_.idxCount = idices.size();
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
        vtxBufBinding_.indices->Release();
        vtxBufBinding_.idxCount = -1;
        vtxBufBinding_.startSlot = -1;
        vtxBufBinding_.bufs.clear();
        vtxBufBinding_.strides.clear();
        vtxBufBinding_.offsets.clear();
    }
}
