/*================================================================
Filename: ActorModel.cpp
Date: 2018.2.5
Created by AirGuanZ
================================================================*/
#include <cassert>
#include <string>

#include "../Utility/ObjFile.h"
#include "../Window/Window.h"
#include "ActorModel.h"

ActorModelComponent::ActorModelComponent(void)
    : idxCount_(0)
{
    
}

bool ActorModelComponent::Initialize(const std::vector<Vertex> vtxData,
                                     const std::vector<UINT> idxData)
{
    assert(!IsAvailable() && vtxData.size() && idxData.size());
    
    if(!vtxBuf_.Initialize(vtxData.data(), sizeof(Vertex) * vtxData.size()) ||
       !idxBuf_.Initialize(idxData.data(), sizeof(UINT) * idxData.size()))
    {
        Destroy();
        return false;
    }
    
    idxCount_ = idxData.size();
    
    return true;
}

void ActorModelComponent::Destroy(void)
{
    vtxBuf_.Destroy();
    idxBuf_.Destroy();
    idxCount_ = 0;
}

bool ActorModelComponent::IsAvailable(void) const
{
    return vtxBuf_.IsAvailable();
}

void ActorModelComponent::Draw(void)
{
    assert(IsAvailable());
    ID3D11DeviceContext *DC = Window::GetInstance().GetD3DDeviceContext();
    ID3D11Buffer *vtxBuf = vtxBuf_.GetBuffer();
    UINT stride = 0, offset = 0;

    DC->IASetVertexBuffers(0, 1, &vtxBuf, &stride, &offset);
    DC->IASetIndexBuffer(idxBuf_.GetBuffer(), DXGI_FORMAT_R16_UINT, 0);

    DC->DrawIndexed(idxCount_, 0, 0);

    vtxBuf = nullptr;
    DC->IASetVertexBuffers(0, 1, &vtxBuf, &stride, &offset);
    DC->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
}

ActorModel::ActorModel(void)
    : state_(ActorModelState::Standing),
      time_(0.0f),
      brightness_(1.0f)
{
    InitState_Standing();
}

namespace
{
    bool LoadActorModelComponent(const std::wstring &filename,
                                 ActorModelComponent &component)
    {
        assert(!component.IsAvailable());

        ObjFile_PNT obj;
        if(!obj.LoadFromFile(filename))
            return false;

        std::vector<ActorModelVertex> vtxData(obj.vertices.size());
        for(int i = 0; i != obj.vertices.size(); ++i)
        {
            const ObjFile_PNT::Vertex &vtx = obj.vertices[i];
            vtxData[i].pos = vtx.pos;
            vtxData[i].nor = vtx.nor;
            vtxData[i].uv = vtx.uv;
        }

        return component.Initialize(vtxData, obj.indices);
    }
}

bool ActorModel::Initialize(void)
{
    //TODO
}

void ActorModel::InitState_Standing(void)
{
    //TODO
}
