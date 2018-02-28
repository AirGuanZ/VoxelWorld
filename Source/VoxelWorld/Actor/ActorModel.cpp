/*================================================================
Filename: ActorModel.cpp
Date: 2018.2.10
Created by AirGuanZ
================================================================*/
#include <cassert>
#include <string>

#include <SkeletonAnimation/SkeletonDataLoader.h>

#include <Utility/HelperFunctions.h>
#include <Utility/ObjFile.h>

#include <Resource/ResourceNameManager.h>
#include <Window/Window.h>
#include "ActorModel.h"

constexpr float ACTOR_MODEL_SCALE_FACTOR = 0.05625f;

namespace
{
    bool InitActorSkeleton(Skeleton::Skeleton &skeleton, std::map<std::string, int> &boneMap)
    {
        std::string errMsg;
        return Skeleton::SkeletonDataLoader::GetInstance().LoadFromVWFile(
            L"output.vwani", 0.35f, 1.0f, skeleton, boneMap, errMsg);
    }

    bool InitActorMesh(const std::map<std::string, int> &boneMap, std::vector<ActorModelComponent> &meshes)
    {
        meshes.clear();

        RscNameMgr &rscMgr = RscNameMgr::GetInstance();
        
        static const std::wstring objFilenames[] =
        {
            rscMgr(u8"ActorModel", u8"DefaultModelHead"),
            rscMgr(u8"ActorModel", u8"DefaultModelBody"),
            rscMgr(u8"ActorModel", u8"DefaultModelLeftHand"),
            rscMgr(u8"ActorModel", u8"DefaultModelRightHand"),
            rscMgr(u8"ActorModel", u8"DefaultModelLeftFoot"),
            rscMgr(u8"ActorModel", u8"DefaultModelRightFoot")
        };
        
        static const std::string boneNames[] =
        {
            u8"Armature_Head",
            u8"Armature_Body",
            u8"Armature_LeftHand",
            u8"Armature_RightHand",
            u8"Armature_LeftFoot",
            u8"Armature_RightFoot"
        };
        
        static_assert(Helper::ArraySize(objFilenames) == Helper::ArraySize(boneNames));
        constexpr int BONE_NUM = static_cast<int>(Helper::ArraySize(boneNames));
        
        meshes.resize(BONE_NUM);
        
        for(int i = 0; i < BONE_NUM; ++i)
        {
            ObjFile_PNT obj;
            if(!obj.LoadFromFile(objFilenames[i], ACTOR_MODEL_SCALE_FACTOR))
            {
                meshes.clear();
                return false;
            }
            
            std::vector<ActorModelVertex> vtxData(obj.vertices.size());
            for(size_t vIdx = 0; vIdx < vtxData.size(); ++vIdx)
            {
                vtxData[vIdx].pos = obj.vertices[vIdx].pos;
                vtxData[vIdx].nor = obj.vertices[vIdx].nor;
                vtxData[vIdx].uv  = obj.vertices[vIdx].uv;
            }
            
            if(!meshes[i].vtxBuf.Initialize(vtxData.data(), sizeof(ActorModelVertex) * vtxData.size()) ||
               !meshes[i].idxBuf.Initialize(obj.indices.data(), sizeof(UINT16) * obj.indices.size()))
            {
                meshes.clear();
                return false;
            }
            
            auto it = boneMap.find(boneNames[i]);
            if(it == boneMap.end())
            {
                meshes.clear();
                return false;
            }
            
            meshes[i].boneIndex = it->second;
            meshes[i].idxCount  = static_cast<int>(obj.indices.size());
        }
        
        return true;
    }
}

bool ActorModel::Initialize(std::string &errMsg)
{
    RscNameMgr &rscMgr = RscNameMgr::GetInstance();

    ID3D11Device *dev = Window::GetInstance().GetD3DDevice();
    ID3D11DeviceContext *DC = Window::GetInstance().GetD3DDeviceContext();

    assert(!tex_.IsAvailable());
    if(!tex_.LoadFromFile(rscMgr(u8"ActorModel", u8"DefaultTexture")))
    {
        errMsg = u8"Failed to load texture for actor renderer";
        Destroy();
        return false;
    }

    if(!sampler_)
    {
        errMsg = u8"Failed to initialize sampler for actor texture";
        Destroy();
        return false;
    }

    assert(!shader_.IsAllStagesAvailable());
    
    std::string VSSrc, PSSrc;
    if(!Helper::ReadFile(rscMgr(u8"ActorModel", u8"VertexShader"), VSSrc) ||
       !Helper::ReadFile(rscMgr(u8"ActorModel", u8"PixelShader"), PSSrc))
    {
        errMsg = u8"Failed to load shader source for actor renderer";
        return false;
    }

    if(!shader_.InitStage<SS_VS>(dev, VSSrc, &errMsg) ||
       !shader_.InitStage<SS_PS>(dev, PSSrc, &errMsg))
        return false;

    uniforms_.reset(shader_.CreateUniformManager());
    if(!uniforms_)
    {
        errMsg = u8"Failed to create shader uniforms manager for actor renderer";
        Destroy();
        return false;
    }
    try
    {
        uniforms_->GetShaderResource<SS_PS>(u8"tex")->SetShaderResource(tex_);
        uniforms_->GetShaderSampler<SS_PS>(u8"sam")->SetSampler(sampler_);
    }
    catch(const OWE::Error &err)
    {
        errMsg = std::string(u8"Error in actor renderer uniforms: ") + err.what();
        Destroy();
        return false;
    }

    D3D11_INPUT_ELEMENT_DESC inputDesc[] =
    {
        { u8"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            Helper::MemOffset(&ActorModelVertex::pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { u8"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            Helper::MemOffset(&ActorModelVertex::nor), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { u8"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
            Helper::MemOffset(&ActorModelVertex::uv), D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    if(!inputLayout_.Initialize(inputDesc,
        shader_.GetShaderByteCodeWithInputSignature(),
        shader_.GetShaderByteCodeSizeWithInputSignature()))
    {
        errMsg = u8"Failed to create input layout for actor renderer";
        Destroy();
        return false;
    }

    worldTrans_ = Matrix::Identity;

    std::map<std::string, int> boneMap;
    if(!InitActorSkeleton(skeleton_, boneMap))
    {
        errMsg = u8"Failed to initialize actor skeleton";
        Destroy();
        return false;
    }

    if(!InitActorMesh(boneMap, meshes_))
    {
        errMsg = u8"Failed to initialize actor meshes";
        Destroy();
        return false;
    }

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
    currentAniClip_ = u8"";
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

    uniforms_->GetConstantBuffer<SS_PS, PSCBBrightness, true>(dev, u8"Brightness")
        ->SetBufferData(DC, { { 1.0f, 1.0f, 1.0f } });

    for(ActorModelComponent &mesh : meshes_)
    {
        Matrix WVP = boneMats[mesh.boneIndex] * worldTrans_
                                              * cam.GetViewProjMatrix();
        uniforms_->GetConstantBuffer<SS_VS, VSCBTrans, true>(dev, u8"Trans")
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

bool ActorModel::End(void) const
{
    //循环动画永无结束之日
    if(aniClipLoop_)
        return false;

    return (t_ >= skeleton_.GetEndTime(currentAniClip_));
}
