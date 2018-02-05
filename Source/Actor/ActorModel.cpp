/*================================================================
Filename: ActorModel.cpp
Date: 2018.2.5
Created by AirGuanZ
================================================================*/
#include <cassert>

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
    
    if(!vtxBuf_.Initialize(vtxData.data(), sizeof(Vertex) * vtxData.size()))
    {
        Destroy();
        return false;
    }
    
    if(!idxBuf_.Initialize(idxData.data(), sizeof(UINT) * idxData.size()))
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

void ActorModelComponent::Draw(void) const
{
    //TODO
}

ActorModel::ActorModel(void)
    : state_(ActorModelState::Standing),
      time_(0.0f),
      brightness_(1.0f)
{
    InitState_Standing();
}

bool ActorModel::Initialize(void)
{
    //TODO
}
