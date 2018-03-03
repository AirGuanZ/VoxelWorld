/*================================================================
Filename: VoxelModelAnimationDisplayer.cpp
Date: 2018.3.3
Created by AirGuanZ
================================================================*/
#include <cassert>
#include <string>

#include <Utility/HelperFunctions.h>

#include <Resource/ResourceNameManager.h>
#include <Window/Window.h>
#include "VoxelModelAnimationDisplayer.h"

VoxelModelAnimationDisplayer::VoxelModelAnimationDisplayer(void)
{
    curAni_  = nullptr;
    aniTime_ = 0.0f;
    aniLoop_ = false;
}

bool VoxelModelAnimationDisplayer::Initialize(
    const std::vector<VoxelModel*> &models,
    const Skeleton::Skeleton *skeleton,
    const std::vector<std::string> &modelBindings,
    const std::map<std::string, int> &boneMap,
    std::string &errMsg)
{
    assert(models.size() == modelBindings.size());
    assert(skeleton != nullptr);

    RscNameMgr &rM = RscNameMgr::GetInstance();
    ID3D11Device *dev = Window::GetInstance().GetD3DDevice();
    errMsg = "";
    Clear();

    models_ = models;
    skeleton_ = skeleton;

    std::string vsSrc, psSrc;
    if(!Helper::ReadFile(rM("VoxelModelAnimationDisplayer", "VertexShader"), vsSrc) ||
       !Helper::ReadFile(rM("VoxelModelAnimationDisplayer", "PixelShader"), psSrc))
    {
        errMsg = "Failed to load shader source for voxel model animation displaying";
        Clear();
        return false;
    }

    if(!shader_.InitStage<SS_VS>(dev, vsSrc, &errMsg) ||
       !shader_.InitStage<SS_PS>(dev, psSrc, &errMsg))
    {
        Clear();
        return false;
    }

    inputLayout_ = VoxelModel::CreateInputLayout(
        shader_.GetShaderByteCodeWithInputSignature(),
        shader_.GetShaderByteCodeSizeWithInputSignature());
    if(!inputLayout_)
    {
        Clear();
        return false;
    }

    uniforms_.reset(shader_.CreateUniformManager());
    if(!uniforms_)
    {
        errMsg = "Failed to create shader uniform manager for "
                 "voxel model animation displaying";
        Clear();
        return false;
    }

    modelToBone_.resize(models_.size());
    for(size_t i = 0; i < models_.size(); ++i)
    {
        auto it = boneMap.find(modelBindings[i]);
        if(it == boneMap.end())
        {
            errMsg = "Bone " + modelBindings[i] + " not found";
            Clear();
            return false;
        }
        modelToBone_[i] = it->second;
    }
}

void VoxelModelAnimationDisplayer::Clear(void)
{
    models_.clear();
    modelToBone_.clear();
    skeleton_ = nullptr;

    curAniName_ = "";
    curAni_  = nullptr;
    aniTime_ = 0.0f;
    aniLoop_ = false;

    shader_.Destroy();
    uniforms_.reset();
    inputLayout_.Destroy();
}

void VoxelModelAnimationDisplayer::SetModel(int idx, VoxelModel *model)
{
    assert(0 <= idx && idx < models_.size() && model != nullptr);
    models_[idx] = model;
}

bool VoxelModelAnimationDisplayer::SetCurrentAnimation(const std::string &aniName, bool loop)
{
    if(!(curAni_ = skeleton_->GetAniClip(aniName)))
        return false;
    aniLoop_ = loop;
    aniTime_ = 0.0f;
    curAniName_ = aniName;
    return true;
}

void VoxelModelAnimationDisplayer::RestartAnimation(void)
{
    aniTime_ = 0.0f;
}

void VoxelModelAnimationDisplayer::Update(float dT)
{
    assert(curAni_ != nullptr);
    aniTime_ += dT;
    if(aniTime_ >= curAni_->End() && aniLoop_)
    {
        while(aniTime_ >= curAni_->End())
            aniTime_ -= (curAni_->Start() - curAni_->End());
    }
}

void VoxelModelAnimationDisplayer::Render(const Matrix &WVP)
{
    ID3D11Device *dev = Window::GetInstance().GetD3DDevice();
    ID3D11DeviceContext *DC = Window::GetInstance().GetD3DDeviceContext();

    std::vector<Matrix> boneMats;
    if(!skeleton_->GetTransMatrix(curAniName_, aniTime_, boneMats))
        return;

    shader_.Bind(DC);
    DC->IASetInputLayout(inputLayout_);

    auto vscbTrans = uniforms_->GetConstantBuffer<SS_VS, VSCBTrans, true>(dev, "Trans");

    for(size_t i = 0; i < models_.size(); ++i)
    {
        VoxelModel *model = models_[i];
        if(!model)
            continue;
        Matrix tWVP = boneMats[modelToBone_[i]] * WVP;
        vscbTrans->SetBufferData(DC, { tWVP.Transpose() });
        uniforms_->Bind(DC);

        model->Bind(DC);
        DC->DrawIndexed(model->GetIndexCount(), 0, 0);
        model->Unbind(DC);
    }

    uniforms_->Unbind(DC);
    DC->IASetInputLayout(nullptr);
    shader_.Unbind(DC);
}
