/*================================================================
Filename: ActorModel.cpp
Date: 2018.2.10
Created by AirGuanZ
================================================================*/
#include <cassert>
#include <string>

#include "../Animation/SkeletonDataLoader.h"
#include "../Resource/ResourceName.h"
#include "../Utility/HelperFunctions.h"
#include "../Utility/ObjFile.h"
#include "../Window/Window.h"
#include "ActorModel.h"

namespace
{
    bool InitActorSkeleton1(Skeleton::Skeleton &skeleton)
    {
        skeleton.Clear();

        std::vector<int> parents = { -1, 0 };
        std::vector<Matrix> offsets =
        {
            Matrix::Identity,
            Matrix::CreateTranslation(0.0f, -1.0f, 0.0f)
        };
        skeleton.Initialize(std::move(parents), std::move(offsets));

        Skeleton::AniClip clip;
        clip.boneAnis.resize(2);
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
        skeleton.AddClip("", std::move(clip));
        
        return true;
    }

    bool InitActorSkeleton(Skeleton::Skeleton &skeleton)
    {
        std::map<std::string, int> boneIdx; std::string errMsg;
        Skeleton::SkeletonDataLoader::GetInstance().LoadFromFile(L"untitled.fbx", 16.6667f, skeleton, boneIdx, errMsg);
        return true;
    }

    bool InitActorMesh(std::vector<ActorModelComponent> &meshes)
    {
        meshes.clear();

        ObjFile_PNT obj;
        if(!obj.LoadFromFile(L"Bin/Model/Actor/actor.obj", 0.02f))
            return false;

        meshes.resize(2);

        std::vector<ActorModelVertex> vtxData(obj.vertices.size());
        for(int i = 0; i != vtxData.size(); ++i)
        {
            vtxData[i].pos = obj.vertices[i].pos;
            vtxData[i].uv = obj.vertices[i].uv;
        }

        if(!meshes[0].vtxBuf.Initialize(vtxData.data(), sizeof(ActorModelVertex) * vtxData.size()) ||
           !meshes[1].vtxBuf.Initialize(vtxData.data(), sizeof(ActorModelVertex) * vtxData.size()) ||
           !meshes[0].idxBuf.Initialize(obj.indices.data(), sizeof(UINT16) * obj.indices.size()) ||
           !meshes[1].idxBuf.Initialize(obj.indices.data(), sizeof(UINT16) * obj.indices.size()))
        {
            meshes.clear();
            return false;
        }

        meshes[0].boneIndex = 0;
        meshes[0].idxCount = obj.indices.size();
        meshes[1].boneIndex = 1;
        meshes[1].idxCount = obj.indices.size();

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

    SetAnimationClip("Armature|Test", true);

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
    if(name != currentAniClip_)
    {
        t_ = 0.0f;
        currentAniClip_ = name;
    }
    aniClipLoop_ = loop;
}

void ActorModel::Update(float deltaT)
{
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

    uniforms_->GetConstantBuffer<SS_PS, PSCBBrightness, true>(dev, "Brightness")
        ->SetBufferData(DC, { { 1.0f, 1.0f, 1.0f } });

    for(ActorModelComponent &mesh : meshes_)
    {
        Matrix WVP = boneMats[mesh.boneIndex] * worldTrans_
                                              * cam.GetViewProjMatrix();
        uniforms_->GetConstantBuffer<SS_VS, VSCBTrans, true>(dev, "Trans")
            ->SetBufferData(DC, { WVP.Transpose() });
        uniforms_->Bind(DC);

        ID3D11Buffer *vtxBuf = mesh.vtxBuf.GetBuffer();
        DC->IASetVertexBuffers(0, 1, &vtxBuf, &stride, &offset);
        DC->IASetIndexBuffer(mesh.idxBuf.GetBuffer(), DXGI_FORMAT_R16_UINT, 0);
        DC->DrawIndexed(mesh.idxCount, 0, 0);

        uniforms_->Unbind(DC);
    }

    ID3D11Buffer *vtxBuf = nullptr;
    stride = 0;
    DC->IASetVertexBuffers(0, 1, &vtxBuf, &stride, &offset);
    DC->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

    DC->IASetInputLayout(nullptr);
    shader_.Unbind(DC);
}
