/*================================================================
Filename: ActorModel.cpp
Date: 2018.2.5
Created by AirGuanZ
================================================================*/
#include <cassert>
#include <string>

#include "../Resource/ResourceName.h"
#include "../Utility/ObjFile.h"
#include "../Utility/HelperFunctions.h"
#include "../Window/Window.h"
#include "ActorModel.h"

ActorRenderer::ActorRenderer(void)
    : inputLayout_(nullptr)
{

}

bool ActorRenderer::Initialize(std::string &errMsg)
{
    assert(inputLayout_ == nullptr);
    errMsg = "";
    ID3D11Device *dev = Window::GetInstance().GetD3DDevice();

    std::string VSSrc, PSSrc;
    if(!Helper::ReadFile(ACTOR_RENDERER_VERTEX_SHADER, VSSrc) ||
       !Helper::ReadFile(ACTOR_RENDERER_PIXEL_SHADER, PSSrc))
    {
        errMsg = "Failed to load shader source file for actor renderer";
        goto FAILED;
    }

    if(!shader_.InitStage<SS_VS>(dev, VSSrc, &errMsg) ||
       !shader_.InitStage<SS_PS>(dev, PSSrc, &errMsg))
    {
        errMsg = "Failed to initialize shader for actor renderer" + errMsg;
        goto FAILED;
    }

    D3D11_INPUT_ELEMENT_DESC inputDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            Helper::MemOffset(&ActorModelVertex::pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            Helper::MemOffset(&ActorModelVertex::nor), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
            Helper::MemOffset(&ActorModelVertex::uv), D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    inputLayout_ = Helper::CreateInputLayout(
        inputDesc, 1,
        shader_.GetShaderByteCodeWithInputSignature(),
        shader_.GetShaderByteCodeSizeWithInputSignature());
    if(!inputLayout_)
        goto FAILED;

    return true;

FAILED:
    shader_.Destroy();
    Helper::ReleaseCOMObjects(inputLayout_);
    return false;
}

void ActorRenderer::Destroy(void)
{
    Helper::ReleaseCOMObjects(inputLayout_);
    shader_.Destroy();
}

void ActorRenderer::Begin(void)
{
    assert(inputLayout_ != nullptr && shader_.IsAllStagesAvailable());
    ID3D11DeviceContext *DC = Window::GetInstance().GetD3DDeviceContext();
    DC->IASetInputLayout(inputLayout_);
    shader_.Bind(DC);
}

void ActorRenderer::End(void)
{
    assert(inputLayout_ != nullptr && shader_.IsAllStagesAvailable());
    ID3D11DeviceContext *DC = Window::GetInstance().GetD3DDeviceContext();
    DC->IASetInputLayout(nullptr);
    shader_.Unbind(DC);
}

ActorRenderer::Shader &ActorRenderer::GetShader(void)
{
    return shader_;
}

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

}

namespace
{
    bool LoadActorModelComponent(const std::wstring &filename,
                                 ActorModelComponent &component)
    {
        if(component.IsAvailable())
            return false;

        ObjFile_PNT obj;
        if(!obj.LoadFromFile(filename))
            return false;

        std::vector<ActorModelVertex> vtxData(obj.vertices.size());
        for(int i = 0; i != obj.vertices.size(); ++i)
        {
            const ObjFile_PNT::Vertex &vtx = obj.vertices[i];
            vtxData[i].pos = vtx.pos;
            vtxData[i].nor = vtx.nor;
            vtxData[i].uv  = vtx.uv;
        }

        return component.Initialize(vtxData, obj.indices);
    }
}

bool ActorModel::Initialize(std::string &errMsg)
{
    errMsg = "";

    if(!LoadActorModelComponent(ACTOR_DEFAULT_HEAD_MODEL, headModel_) ||
       !LoadActorModelComponent(ACTOR_DEFAULT_BODY_MODEL, bodyModel_) ||
       !LoadActorModelComponent(ACTOR_DEFAULT_LEFT_HAND_MODEL, leftHandModel_) ||
       !LoadActorModelComponent(ACTOR_DEFAULT_RIGHT_HAND_MODEL, rightHandModel_) ||
       !LoadActorModelComponent(ACTOR_DEFAULT_LEFT_FOOT_MODEL, leftFootModel_) ||
       !LoadActorModelComponent(ACTOR_DEFAULT_RIGHT_FOOT_MODEL, rightFootModel_))
    {
        errMsg = "Failed to load actor model from file";
        Destroy();
        return false;
    }

    if(!headTex_.LoadFromFile(ACTOR_DEFAULT_HEAD_TEXTURE) ||
       !bodyTex_.LoadFromFile(ACTOR_DEFAULT_BODY_TEXTURE) ||
       !leftHandTex_.LoadFromFile(ACTOR_DEFAULT_LEFT_HAND_TEXTURE) ||
       !rightHandTex_.LoadFromFile(ACTOR_DEFAULT_RIGHT_HAND_TEXTURE) ||
       !leftFootTex_.LoadFromFile(ACTOR_DEFAULT_LEFT_FOOT_TEXTURE) ||
       !rightFootTex_.LoadFromFile(ACTOR_DEFAULT_RIGHT_FOOT_TEXTURE))
    {
        errMsg = "Failed to load actor texture from file";
        Destroy();
        return false;
    }

    if(!sampler_)
    {
        errMsg = "Failed to initialize texture sampler for actor rendering";
        Destroy();
        return false;
    }

    if(!renderer_.Initialize(errMsg))
    {
        Destroy();
        return false;
    }

    uniforms_.reset(renderer_.GetShader().CreateUniformManager());
    if(!uniforms_)
    {
        errMsg = "Failed to create shader uniform manager for actor rendering";
        Destroy();
        return false;
    }

    InitState_Standing();

    return true;
}

void ActorModel::SetState(ActorModelState state)
{
    switch(state)
    {
    case ActorModelState::Standing:
        InitState_Standing();
        break;
    case ActorModelState::Running:
        InitState_Running();
        break;
    default:
        std::abort();
    }
}

void ActorModel::Update(float deltaT)
{
    switch(state_)
    {
    case ActorModelState::Standing:
        UpdateState_Standing(deltaT);
        break;
    case ActorModelState::Running:
        UpdateState_Running(deltaT);
        break;
    default:
        std::abort();
    }
}

void ActorModel::SetBrightness(float brightness)
{
    brightness_ = brightness;
}

void ActorModel::Render(const Camera &cam, const Matrix &worldTrans)
{
    struct VSCBTrans
    {
        Matrix WVP;
    };

    struct PSCBBrightness
    {
        alignas(16) float brightness;
    };

    ID3D11Device *dev = Window::GetInstance().GetD3DDevice();
    ID3D11DeviceContext *DC = Window::GetInstance().GetD3DDeviceContext();

    auto vscbTrans = uniforms_->GetConstantBuffer<SS_VS, VSCBTrans, true>
        (dev, "Trans");
    auto psTex = uniforms_->GetShaderResource<SS_PS>("tex");

    uniforms_->GetConstantBuffer<SS_PS, PSCBBrightness, true>(dev, "Brightness")
        ->SetBufferData(DC, { brightness_ });
    uniforms_->GetShaderSampler<SS_PS>("sam")->SetSampler(sampler_);

    auto RenderComponent = [&](const Matrix &localTrans, Texture2D &tex, ActorModelComponent &model)
    {
        vscbTrans->SetBufferData(DC, { (localTrans * worldTrans).Transpose() });
        psTex->SetShaderResource(tex);
        uniforms_->Bind(DC);
        model.Draw();
    };

    renderer_.Begin();
    {
        RenderComponent(headLocalTrans_,      headTex_,      headModel_);
        RenderComponent(bodyLocalTrans_,      bodyTex_,      bodyModel_);
        RenderComponent(leftHandLocalTrans_,  leftHandTex_,  leftHandModel_);
        RenderComponent(rightHandLocalTrans_, rightHandTex_, rightHandModel_);
        RenderComponent(leftFootLocalTrans_,  leftFootTex_,  leftFootModel_);
        RenderComponent(rightFootLocalTrans_, rightFootTex_, rightFootModel_);

        uniforms_->Unbind(DC);
    }
    renderer_.End();
}

void ActorModel::Destroy(void)
{
    state_ = ActorModelState::Standing;
    time_ = 0.0f;
    brightness_ = 1.0f;

    headModel_.Destroy();
    bodyModel_.Destroy();
    leftHandModel_.Destroy();
    rightHandModel_.Destroy();
    leftFootModel_.Destroy();
    rightFootModel_.Destroy();

    headLocalTrans_ = Matrix::Identity;
    bodyLocalTrans_ = Matrix::Identity;
    leftHandLocalTrans_ = Matrix::Identity;
    rightHandLocalTrans_ = Matrix::Identity;
    leftFootLocalTrans_ = Matrix::Identity;
    rightFootLocalTrans_ = Matrix::Identity;

    headTex_.Destroy();
    bodyTex_.Destroy();
    leftHandTex_.Destroy();
    rightHandTex_.Destroy();
    leftFootTex_.Destroy();
    rightFootTex_.Destroy();

    renderer_.Destroy();
    uniforms_.reset();
}

void ActorModel::InitState_Standing(void)
{
    state_ = ActorModelState::Standing;
    time_ = 0.0f;

    headLocalTrans_      = Matrix::CreateTranslation({ 0.0f, 0.0f, 0.0f });
    bodyLocalTrans_      = Matrix::CreateTranslation({ 0.0f, -1.6f, 0.0f });
    leftHandLocalTrans_  = Matrix::CreateTranslation({ -0.2f, -0.6f, 0.0f });
    rightHandLocalTrans_ = Matrix::CreateTranslation({ 0.2f, -0.6f, 0.0f });
    leftFootLocalTrans_  = Matrix::CreateTranslation({ -0.2f, -1.6f, 0.0f });
    rightFootLocalTrans_ = Matrix::CreateTranslation({ 0.2f, -1.6f, 0.0f });
}

void ActorModel::InitState_Running(void)
{
    //TODO
    InitState_Standing();
}

void ActorModel::UpdateState_Standing(float deltaT)
{
    //TODO
}

void ActorModel::UpdateState_Running(float deltaT)
{
    //TODO
    UpdateState_Standing(deltaT);
}
