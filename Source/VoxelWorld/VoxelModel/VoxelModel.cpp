/*================================================================
Filename: VoxelModel.cpp
Date: 2018.3.3
Created by AirGuanZ
================================================================*/
#include <cassert>

#include <Utility/HelperFunctions.h>

#include "VoxelModel.h"

InputLayout VoxelModel::CreateInputLayout(const void *shaderByteCode, UINT length)
{
    static const D3D11_INPUT_ELEMENT_DESC desc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            Helper::MemOffset(&Vertex::pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            Helper::MemOffset(&Vertex::color), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            Helper::MemOffset(&Vertex::nor), D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    InputLayout rt;
    rt.Initialize(desc, shaderByteCode, length);
    return rt;
}

bool VoxelModel::Initialize(const std::vector<VoxelModelVertex> &vtx, const std::vector<UINT16> &idx)
{
    Destroy();
    if(!vtxBuf_.Initialize(vtx.data(), sizeof(Vertex) * vtx.size()) ||
       !idxBuf_.Initialize(idx.data(), sizeof(UINT16) * idx.size()))
    {
        Destroy();
        return false;
    }
    return true;
}

void VoxelModel::Destroy(void)
{
    vtxBuf_.Destroy();
    idxBuf_.Destroy();
}

bool VoxelModel::IsAvailable(void) const
{
    assert(vtxBuf_.IsAvailable() == idxBuf_.IsAvailable());
    return vtxBuf_.IsAvailable();
}

void VoxelModel::Bind(ID3D11DeviceContext *DC)
{
    assert(DC != nullptr && IsAvailable());
    UINT stride = sizeof(Vertex), offset = 0;
    ID3D11Buffer *vtxBuf = vtxBuf_;

    DC->IASetVertexBuffers(0, 1, &vtxBuf, &stride, &offset);
    DC->IASetIndexBuffer(idxBuf_, DXGI_FORMAT_R16_UINT, 0);
}

void VoxelModel::Unbind(ID3D11DeviceContext *DC)
{
    assert(DC != nullptr);
    UINT stride = sizeof(Vertex), offset = 0;
    ID3D11Buffer *vtxBuf = nullptr;
    DC->IASetVertexBuffers(0, 1, &vtxBuf, &stride, &offset);
    DC->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
}
