/*================================================================
Filename: ActorModel.cpp
Date: 2018.2.10
Created by AirGuanZ
================================================================*/
#include <cassert>
#include <string>

#include "../Resource/ResourceName.h"
#include "../Utility/HelperFunctions.h"
#include "../Window/Window.h"
#include "ActorModel.h"

namespace
{
    bool InitActorSkeleton(Skeleton::Skeleton &skeleton)
    {
        skeleton.Clear();

        std::vector<int> parents = { 0, 0 };
        std::vector<Matrix> offsets =
        {
            Matrix::Identity,
            Matrix::CreateTranslation(0.0f, -1.0f, 0.0f)
        };
        skeleton.Initialize(std::move(parents), std::move(offsets));

        Skeleton::AniClip clip;
        clip.boneAnis.resize(2);
        clip.boneAnis[0].keyframes =
        {
            {
                0.0f,
                Vector3(1.0f, 1.0f, 1.0f),
                Vector3(0.0f, 0.0f, 0.0f),
                Quaternion::Identity
            },
            {
                1.0f,
                Vector3(1.0f, 1.0f, 1.0f),
                Vector3(0.0f, 0.0f, 0.0f),
                Quaternion::Identity
            }
        };
        clip.boneAnis[1].keyframes =
        {
            {
                0.0f,
                Vector3(1.0f, 1.0f, 1.0f),
                Vector3(0.0f, 0.0f, 0.0f),
                Quaternion::Identity
            },
            {
                1000.0f,
                Vector3(1.0f, 1.0f, 1.0f),
                Vector3(0.0f, -3.0f, 0.0f),
                Quaternion::CreateFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), 3.14159f)
            },
            {
                2000.0f,
                Vector3(1.0f, 1.0f, 1.0f),
                Vector3(0.0f, 0.0f, 0.0f),
                Quaternion::Identity
            }
        };
        clip.UpdateStartEndTime();
        skeleton.AddClip("test", std::move(clip));
        
        return true;
    }

    bool InitActorMesh(std::vector<ActorModelComponent> &meshes)
    {
        meshes.clear();

        float halfSize = 0.5f;

        std::vector<ActorModelVertex> vtxData =
        {
            //x+
            { { +halfSize, -halfSize, +halfSize }, { 0.0f, 1.0f } },
            { { +halfSize, +halfSize, +halfSize }, { 0.0f, 0.0f } },
            { { +halfSize, +halfSize, -halfSize }, { 1.0f, 0.0f } },
            { { +halfSize, -halfSize, +halfSize }, { 0.0f, 1.0f } },
            { { +halfSize, +halfSize, -halfSize }, { 1.0f, 0.0f } },
            { { +halfSize, -halfSize, -halfSize }, { 1.0f, 1.0f } },
                                                   
            //x-                                                 
            { { -halfSize, -halfSize, -halfSize }, { 0.0f, 1.0f } },
            { { -halfSize, +halfSize, -halfSize }, { 0.0f, 0.0f } },
            { { -halfSize, +halfSize, +halfSize }, { 1.0f, 0.0f } },
            { { -halfSize, -halfSize, -halfSize }, { 0.0f, 1.0f } },
            { { -halfSize, +halfSize, +halfSize }, { 1.0f, 0.0f } },
            { { -halfSize, -halfSize, +halfSize }, { 1.0f, 1.0f } },
                                                   
            //y+                                                 
            { { -halfSize, +halfSize, +halfSize }, { 0.0f, 1.0f } },
            { { -halfSize, +halfSize, -halfSize }, { 0.0f, 0.0f } },
            { { +halfSize, +halfSize, -halfSize }, { 1.0f, 0.0f } },
            { { -halfSize, +halfSize, +halfSize }, { 0.0f, 1.0f } },
            { { +halfSize, +halfSize, -halfSize }, { 1.0f, 0.0f } },
            { { +halfSize, +halfSize, +halfSize }, { 1.0f, 1.0f } },
                                                   
            //y-                                                 
            { { -halfSize, -halfSize, -halfSize }, { 0.0f, 1.0f } },
            { { -halfSize, -halfSize, +halfSize }, { 0.0f, 0.0f } },
            { { +halfSize, -halfSize, +halfSize }, { 1.0f, 0.0f } },
            { { -halfSize, -halfSize, -halfSize }, { 0.0f, 1.0f } },
            { { +halfSize, -halfSize, +halfSize }, { 1.0f, 0.0f } },
            { { +halfSize, -halfSize, -halfSize }, { 1.0f, 1.0f } },
                                                   
            //z-                                                 
            { { +halfSize, -halfSize, -halfSize }, { 0.0f, 1.0f } },
            { { +halfSize, +halfSize, -halfSize }, { 0.0f, 0.0f } },
            { { -halfSize, +halfSize, -halfSize }, { 1.0f, 0.0f } },
            { { +halfSize, -halfSize, -halfSize }, { 0.0f, 1.0f } },
            { { -halfSize, +halfSize, -halfSize }, { 1.0f, 0.0f } },
            { { -halfSize, -halfSize, -halfSize }, { 1.0f, 1.0f } },
                                                   
            //z+                                                 
            { { -halfSize, -halfSize, +halfSize }, { 0.0f, 1.0f } },
            { { -halfSize, +halfSize, +halfSize }, { 0.0f, 0.0f } },
            { { +halfSize, +halfSize, +halfSize }, { 1.0f, 0.0f } },
            { { -halfSize, -halfSize, +halfSize }, { 0.0f, 1.0f } },
            { { +halfSize, +halfSize, +halfSize }, { 1.0f, 0.0f } },
            { { +halfSize, -halfSize, +halfSize }, { 1.0f, 1.0f } }
        };

        std::vector<UINT> idxData(vtxData.size());
        for(UINT i = 0; i != idxData.size(); ++i)
            idxData[i] = i;

        meshes.resize(2);

        if(!meshes[0].vtxBuf.Initialize(
                vtxData.data(), sizeof(ActorModelVertex) * vtxData.size()) ||
           !meshes[0].idxBuf.Initialize(
                idxData.data(), sizeof(UINT) * idxData.size()))
        {
            meshes.clear();
            return false;
        }
        meshes[0].boneIndex = 0;
        meshes[0].idxCount = idxData.size();

        if(!meshes[1].vtxBuf.Initialize(
                vtxData.data(), sizeof(ActorModelVertex) * vtxData.size()) ||
           !meshes[1].idxBuf.Initialize(
                idxData.data(), sizeof(UINT) * idxData.size()))
        {
            meshes.clear();
            return false;
        }
        meshes[1].boneIndex = 1;
        meshes[1].idxCount = idxData.size();

        return true;
    }
}

bool ActorModel::Initialize(std::string &errMsg)
{
    ID3D11Device *dev = Window::GetInstance().GetD3DDevice();
    ID3D11DeviceContext *DC = Window::GetInstance().GetD3DDeviceContext();

    assert(!tex_.IsAvailable());
    if(!tex_.LoadFromFile(ACTOR_RENDERER_TEXTURE))
    {
        errMsg = "Failed to load texture for actor renderer";
        Destroy();
        return false;
    }

    if(!sampler_)
    {
        errMsg = "Failed to initialize sampler for actor texture";
        Destroy();
        return false;
    }

    assert(!shader_.IsAllStagesAvailable());
    
    std::string VSSrc, PSSrc;
    if(!Helper::ReadFile(ACTOR_RENDERER_VERTEX_SHADER, VSSrc) ||
       !Helper::ReadFile(ACTOR_RENDERER_PIXEL_SHADER, PSSrc))
    {
        errMsg = "Failed to load shader source for actor renderer";
        return false;
    }

    if(!shader_.InitStage<SS_VS>(dev, VSSrc, &errMsg) ||
       !shader_.InitStage<SS_PS>(dev, PSSrc, &errMsg))
        return false;

    uniforms_.reset(shader_.CreateUniformManager());
    if(!uniforms_)
    {
        errMsg = "Failed to create shader uniforms manager for actor renderer";
        Destroy();
        return false;
    }
    try
    {
        uniforms_->GetShaderResource<SS_PS>("tex")->SetShaderResource(tex_);
        uniforms_->GetShaderSampler<SS_PS>("sam")->SetSampler(sampler_);
    }
    catch(const OWE::Error &err)
    {
        errMsg = std::string("Error in actor renderer uniforms: ") + err.what();
        Destroy();
        return false;
    }

    D3D11_INPUT_ELEMENT_DESC inputDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            Helper::MemOffset(&ActorModelVertex::pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
            Helper::MemOffset(&ActorModelVertex::uv), D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    if(!inputLayout_.Initialize(inputDesc,
        shader_.GetShaderByteCodeWithInputSignature(),
        shader_.GetShaderByteCodeSizeWithInputSignature()))
    {
        errMsg = "Failed to create input layout for actor renderer";
        Destroy();
        return false;
    }

    worldTrans_ = Matrix::Identity;

    if(!InitActorMesh(meshes_))
    {
        errMsg = "Failed to initialize actor meshes";
        Destroy();
        return false;
    }

    if(!InitActorSkeleton(skeleton_))
    {
        errMsg = "Failed to initialize actor skeleton";
        Destroy();
        return false;
    }

    SetAnimationClip("test", true);

    return true;
}

void ActorModel::Destroy(void)
{
    shader_.Destroy();
    uniforms_.reset();
    inputLayout_ = InputLayout();

    tex_.Destroy();
    
    worldTrans_ = Matrix::Identity;

    meshes_.clear();

    skeleton_.Clear();
    currentAniClip_ = "";
    aniClipLoop_ = false;
    t_ = 0.0f;
}

void ActorModel::SetTransform(const Matrix &mat)
{
    worldTrans_ = mat;
}

void ActorModel::SetAnimationClip(const std::string &name, bool loop)
{
    currentAniClip_ = name;
    aniClipLoop_ = loop;
    t_ = 0.0f;
}

void ActorModel::Update(float deltaT)
{
    if(currentAniClip_ == "")
        return;

    const Skeleton::AniClip *clip = skeleton_.GetAniClip(currentAniClip_);
    if(!clip)
        return;
    t_ += deltaT;
    if(aniClipLoop_)
    {
        while(t_ > clip->End())
            t_ -= (clip->End() - clip->Start());
    }
}

void ActorModel::Render(const Camera &cam)
{
    struct VSCBTrans
    {
        Matrix WVP;
    };

    struct PSCBBrightness
    {
        alignas(16) Vector3 brightness;
    };

    ID3D11Device *dev = Window::GetInstance().GetD3DDevice();
    ID3D11DeviceContext *DC = Window::GetInstance().GetD3DDeviceContext();

    std::vector<Matrix> boneMats;
    if(!skeleton_.GetTransMatrix(currentAniClip_, t_, boneMats))
        return;

    shader_.Bind(DC);
    DC->IASetInputLayout(inputLayout_);
    UINT stride = sizeof(ActorModelVertex), offset = 0;

    for(ActorModelComponent &mesh : meshes_)
    {
        Matrix WVP = boneMats[mesh.boneIndex] * worldTrans_
                                              * cam.GetViewProjMatrix();
        uniforms_->GetConstantBuffer<SS_VS, VSCBTrans, true>(dev, "Trans")
            ->SetBufferData(DC, { WVP.Transpose() });
        uniforms_->GetConstantBuffer<SS_PS, PSCBBrightness, true>(dev, "Brightness")
            ->SetBufferData(DC, { { 1.0f, 1.0f, 1.0f } });
        uniforms_->Bind(DC);

        ID3D11Buffer *vtxBuf = mesh.vtxBuf.GetBuffer();
        DC->IASetVertexBuffers(0, 1, &vtxBuf, &stride, &offset);
        DC->IASetIndexBuffer(mesh.idxBuf.GetBuffer(), DXGI_FORMAT_R16_UINT, 0);
        //DC->DrawIndexed(mesh.idxCount, 0, 0);
        DC->Draw(36, 0);
    }

    ID3D11Buffer *vtxBuf = nullptr;
    stride = 0;
    DC->IASetVertexBuffers(0, 1, &vtxBuf, &stride, &offset);
    //DC->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

    uniforms_->Unbind(DC);
    DC->IASetInputLayout(nullptr);
    shader_.Unbind(DC);
}
