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

bool BasicModel::MakeVertexBuffer(void)
{
    assert(vtxBufBinding_.startSlot == -1);

    //¿ÕmodelÊÇÔÊÐíµÄ
    if(vertices_.empty())
    {
        vtxBufBinding_.startSlot = 0;
        return true;
    }

    assert(indices_.size() % 3 == 0);
    ID3D11Buffer *buf = Helper::CreateVertexBuffer(
        vertices_.data(), vertices_.size() * sizeof(Vertex), false);
    if(!buf)
        return false;

    ID3D11Buffer *idxBuf = Helper::CreateIndexBuffer(
        indices_.data(), indices_.size() * sizeof(UINT16), false);
    if(!idxBuf)
    {
        Helper::ReleaseCOMObjects(buf);
        return false;
    }

    vtxBufBinding_.startSlot = 0;
    vtxBufBinding_.idxCount = indices_.size();
    vtxBufBinding_.indices = idxBuf;
    vtxBufBinding_.indicesFormat = DXGI_FORMAT_R16_UINT;
    vtxBufBinding_.bufs.push_back(buf);
    vtxBufBinding_.strides.push_back(sizeof(Vertex));
    vtxBufBinding_.offsets.push_back(0);

    vertices_.clear();
    indices_.clear();

    return true;
}

void BasicModel::Destroy(void)
{
    vertices_.clear();
    indices_.clear();
    if(IsAvailable())
    {
        assert(vtxBufBinding_.bufs.size() == 1 && vtxBufBinding_.bufs[0]);
        vtxBufBinding_.bufs[0]->Release();

        assert(vtxBufBinding_.indices != nullptr);
        vtxBufBinding_.indices->Release();
        vtxBufBinding_.indices = nullptr;

        vtxBufBinding_.startSlot = -1;
        vtxBufBinding_.idxCount  = -1;
        vtxBufBinding_.bufs.clear();
        vtxBufBinding_.strides.clear();
        vtxBufBinding_.offsets.clear();
    }
}
