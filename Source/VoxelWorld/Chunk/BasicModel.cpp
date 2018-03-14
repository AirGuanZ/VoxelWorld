/*================================================================
Filename: BasicModel.cpp
Date: 2018.1.15
Created by AirGuanZ
================================================================*/
#include <cassert>

#include <Utility\HelperFunctions.h>

#include <Window\Window.h>
#include "BasicModel.h"

namespace
{
    ID3D11Buffer *CreateVertexBuffer(void *initData, size_t byteSize, bool dynamic)
    {
        assert(dynamic || initData);

        D3D11_BUFFER_DESC dc;
        dc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        dc.ByteWidth = static_cast<UINT>(byteSize);
        dc.CPUAccessFlags = dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
        dc.MiscFlags = 0;
        dc.StructureByteStride = 0;
        dc.Usage = dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;

        D3D11_SUBRESOURCE_DATA data = { initData, 0, 0 };

        ID3D11Buffer *rt = nullptr;
        HRESULT hr = Window::GetInstance().GetD3DDevice()->CreateBuffer(
            &dc, initData ? &data : nullptr, &rt);

        return SUCCEEDED(hr) ? rt : nullptr;
    }

    ID3D11Buffer *CreateIndexBuffer(void *initData, size_t byteSize, bool dynamic)
    {
        assert(dynamic || initData);

        D3D11_BUFFER_DESC dc;
        dc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        dc.ByteWidth = static_cast<UINT>(byteSize);
        dc.CPUAccessFlags = dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
        dc.MiscFlags = 0;
        dc.StructureByteStride = 0;
        dc.Usage = dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;

        D3D11_SUBRESOURCE_DATA data = { initData, 0, 0 };

        ID3D11Buffer *rt = nullptr;
        HRESULT hr = Window::GetInstance().GetD3DDevice()->CreateBuffer(
            &dc, initData ? &data : nullptr, &rt);

        return SUCCEEDED(hr) ? rt : nullptr;
    }
}

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
    ID3D11Buffer *buf = CreateVertexBuffer(
        vertices_.data(), vertices_.size() * sizeof(Vertex), false);
    if(!buf)
        return false;

    ID3D11Buffer *idxBuf = CreateIndexBuffer(
        indices_.data(), indices_.size() * sizeof(UINT16), false);
    if(!idxBuf)
    {
        Helper::ReleaseCOMObjects(buf);
        return false;
    }

    vtxBufBinding_.startSlot = 0;
    vtxBufBinding_.idxCount = static_cast<int>(indices_.size());
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
