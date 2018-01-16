/*================================================================
Filename: BasicModel.cpp
Date: 2018.1.15
Created by AirGuanZ
================================================================*/
#include <cassert>

#include "../Utility/HelperFunctions.h"
#include "BasicModel.h"

BasicModel::BasicModel(void)
{

}

BasicModel::~BasicModel(void)
{
    Destroy();
}

void BasicModel::AddTriangle(const BasicModel::Vertex *triangle)
{
    assert(triangle != nullptr);
    triangles_.push_back(triangle[0]);
    triangles_.push_back(triangle[1]);
    triangles_.push_back(triangle[2]);
}

bool BasicModel::MakeVertexBuffer(void)
{
    assert(vtxBufBinding_.startSlot == -1);

    //¿ÕmodelÊÇÔÊÐíµÄ
    if(triangles_.empty())
    {
        vtxBufBinding_.startSlot = 0;
        return true;
    }

    assert(triangles_.size() % 3 == 0);
    ID3D11Buffer *buf = Helper::CreateVertexBuffer(
        triangles_.data(), triangles_.size() * sizeof(Vertex), false);
    if(!buf)
        return false;

    vtxBufBinding_.startSlot = 0;
    vtxBufBinding_.vtxCount = triangles_.size() * 3;
    vtxBufBinding_.bufs.push_back(buf);
    vtxBufBinding_.strides.push_back(sizeof(Vertex));
    vtxBufBinding_.offsets.push_back(0);
    triangles_.clear();

    return true;
}

void BasicModel::Destroy(void)
{
    triangles_.clear();
    if(IsAvailable())
    {
        assert(vtxBufBinding_.bufs.size() == 1 && vtxBufBinding_.bufs[0]);
        vtxBufBinding_.bufs[0]->Release();
        vtxBufBinding_.startSlot = -1;
        vtxBufBinding_.vtxCount  = -1;
        vtxBufBinding_.bufs.clear();
        vtxBufBinding_.strides.clear();
        vtxBufBinding_.offsets.clear();
    }
}
