/*================================================================
Filename: VoxelModel.cpp
Date: 2018.3.3
Created by AirGuanZ
================================================================*/
#define _CRT_SECURE_NO_WARNINGS

#include <cassert>
#include <cstdio>
#include <fstream>

#include <Utility\HelperFunctions.h>

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

bool VoxelModel::LoadFromFile(const std::wstring &filename, VoxelModel &model)
{
    model.Destroy();

    std::ifstream fin(filename, std::ios_base::in);
    if(!fin)
        return false;

    std::string line;
    std::vector<VoxelModelVertex> vtxData;
    std::vector<UINT16> idxData;

    VoxelModelVertex vtx;
    int idx0, idx1, idx2;

    while(std::getline(fin, line))
    {
        if(line.empty() || line[0] == '#')
            continue;
        if(std::sscanf(line.data(), "vtx %f %f %f %f %f %f %f %f %f",
                       &vtx.pos.x, &vtx.pos.y, &vtx.pos.z,
                       &vtx.color.x, &vtx.color.y, &vtx.color.z,
                       &vtx.nor.x, &vtx.nor.y, &vtx.nor.z) == 9)
        {
            vtxData.push_back(vtx);
        }
        else if(std::sscanf(line.data(), "idx %d %d %d",
                            &idx0, &idx1, &idx2) == 3)
        {
            idxData.push_back(static_cast<UINT16>(idx0));
            idxData.push_back(static_cast<UINT16>(idx1));
            idxData.push_back(static_cast<UINT16>(idx2));
        }
        else
            return false;
    }

    if(!model.Initialize(vtxData, idxData))
        return false;

    return true;
}

bool VoxelModel::SaveToFile(const std::wstring &filename,
                            const std::vector<VoxelModelVertex> &vtxData,
                            const std::vector<UINT16> &idxData)
{
    std::ofstream fout(filename, std::ios_base::out | std::ios_base::trunc);
    if(!fout)
        return false;

    fout.precision(8);
    for(auto &vtx : vtxData)
    {
        fout << "vtx "
             << vtx.pos.x << " "   << vtx.pos.y << " "   << vtx.pos.z << " "
             << vtx.color.x << " " << vtx.color.y << " " << vtx.color.z << " "
             << vtx.nor.x << " "   << vtx.nor.y << " "   << vtx.nor.z
             << std::endl;
    }

    assert(idxData.size() % 3 == 0);
    for(size_t i = 0; i < idxData.size(); i += 3)
    {
        fout << "idx "
            << idxData[i] << " " << idxData[i + 1] << " " << idxData[i + 2]
            << std::endl;
    }

    return true;
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
    idxCount_ = idx.size();
    return true;
}

void VoxelModel::Destroy(void)
{
    vtxBuf_.Destroy();
    idxBuf_.Destroy();
    idxCount_ = 0;
}

bool VoxelModel::IsAvailable(void) const
{
    assert(vtxBuf_.IsAvailable() == idxBuf_.IsAvailable());
    return vtxBuf_.IsAvailable();
}

UINT VoxelModel::GetIndexCount(void) const
{
    return static_cast<UINT>(idxCount_);
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
