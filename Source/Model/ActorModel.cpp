/*================================================================
Filename: ActorModel.cpp
Date: 2018.2.2
Created by AirGuanZ
================================================================*/
#include <cassert>

#include "../Utility/HelperFunctions.h"
#include "../Window/Window.h"
#include "ActorModel.h"

void ActorSubmodel::AddVertex(const ActorSubmodel::Vertex &vtx)
{
    vtxData_.push_back(vtx);
}

void ActorSubmodel::AddIndex(UINT index)
{
    idxData_.push_back(index);
}

bool ActorSubmodel::MakeBuffer(void)
{
    assert(vtxData_.size() && idxData_.size());

    DestroyBuffer();

    if(!vtxBuf_.Initialize(vtxData_.data(), sizeof(Vertex) * vtxData_.size()) ||
       !idxBuf_.Initialize(idxData_.data(), sizeof(UINT) * idxData_.size()))
    {
        DestroyBuffer();
        return false;
    }
    return true;
}

void ActorSubmodel::DestroyBuffer(void)
{
    vtxBuf_.Destroy();
    idxBuf_.Destroy();
    idxCount_ = 0;
}

void ActorSubmodel::Clear(void)
{
    DestroyBuffer();
    vtxData_.clear();
    idxData_.clear();
}

bool ActorSubmodel::IsAvailable(void) const
{
    return vtxBuf_.IsAvailable() && idxBuf_.IsAvailable();
}

void ActorSubmodel::Bind(void)
{
    assert(vtxBuf_.IsAvailable() && idxBuf_.IsAvailable());
    ID3D11DeviceContext *DC = Window::GetInstance().GetD3DDeviceContext();
    UINT stride = sizeof(Vertex), offset = 0;
    ID3D11Buffer *vtxBuf = vtxBuf_.GetBuffer();
    DC->IASetVertexBuffers(0, 1, &vtxBuf, &stride, &offset);
    DC->IASetIndexBuffer(idxBuf_.GetBuffer(), DXGI_FORMAT_R16_UINT, 0);
}

void ActorSubmodel::Draw(void)
{
    Window::GetInstance().GetD3DDeviceContext()->DrawIndexed(idxCount_, 0, 0);
}

void ActorSubmodel::Unbind(void)
{
    assert(vtxBuf_.IsAvailable() && idxBuf_.IsAvailable());
    ID3D11DeviceContext *DC = Window::GetInstance().GetD3DDeviceContext();
    UINT stride = 0, offset = 0;
    ID3D11Buffer *vtxBuf = nullptr;
    DC->IASetVertexBuffers(0, 1, &vtxBuf, &stride, &offset);
    DC->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
}

void ActorComponent::SetTransform(const Matrix &world)
{
    world_ = world;
    if(parent_)
        finalWorld_ = parent_->GetFinalTransform() * world;
    else
        finalWorld_ = world_;
    for(ActorComponent *child : children_)
        child->SetTransform(child->world_);
}

void ActorComponent::AddChild(ActorComponent *child)
{
    assert(child != nullptr && child->parent_ == nullptr);

    child->parent_ = this;
    children_.push_back(child);
    child->finalWorld_ = finalWorld_ * child->world_;
}

const Matrix &ActorComponent::GetFinalTransform(void) const
{
    return finalWorld_;
}

ActorSubmodel &ActorComponent::GetModel(void)
{
    return model_;
}

void ActorComponent::Render(const Matrix &viewProj, ActorRenderer::Uniforms *uniforms)
{
    struct VSCBTrans
    {
        Matrix WVP;
    };

    ID3D11Device *dev = Window::GetInstance().GetD3DDevice();
    ID3D11DeviceContext *DC = Window::GetInstance().GetD3DDeviceContext();

    if(model_.IsAvailable())
    {
        uniforms->GetConstantBuffer<SS_VS, VSCBTrans>(dev, "Trans")
            ->SetBufferData(DC, { (finalWorld_ * viewProj).Transpose() });
        uniforms->Bind(DC);
        model_.Bind();
        model_.Draw();
        model_.Unbind();
        uniforms->Unbind(DC);
    }

    for(ActorComponent *child : children_)
        child->Render(viewProj, uniforms);
}

void ActorModel::Render(const Matrix &viewProj, ActorRenderer::Uniforms *uniforms)
{
    root_.Render(viewProj, uniforms);
}

void ActorModel::AddComponent(ActorComponent *component)
{
    assert(component != nullptr);
    root_.AddChild(component);
}

void ActorModel::SetTransform(const Matrix &world)
{
    root_.SetTransform(world);
}
