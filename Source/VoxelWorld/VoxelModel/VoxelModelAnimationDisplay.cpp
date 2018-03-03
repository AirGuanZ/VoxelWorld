/*================================================================
Filename: VoxelModelAnimation.cpp
Date: 2018.3.3
Created by AirGuanZ
================================================================*/
#include <cassert>
#include <string>

#include <Utility/HelperFunctions.h>

#include <Resource/ResourceNameManager.h>
#include <Window/Window.h>
#include "VoxelModelAnimationDisplay.h"

VoxelModelAnimationDisplay::VoxelModelAnimationDisplay(void)
{
    curAni_  = nullptr;
    aniTime_ = 0.0f;
    aniLoop_ = false;
}

bool VoxelModelAnimationDisplay::Initialize(
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
    if(!Helper::ReadFile(rM("VoxelModelEditor", "VertexShader"), vsSrc) ||
       !Helper::ReadFile(rM("VoxelModelEditor", "PixelShader"), psSrc))
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

void VoxelModelAnimationDisplay::Clear(void)
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

void VoxelModelAnimationDisplay::SetModel(int idx, VoxelModel *model)
{
    assert(0 <= idx && idx < models_.size() && model != nullptr);
    models_[idx] = model;
}

bool VoxelModelAnimationDisplay::SetCurrentAnimation(const std::string &aniName, bool loop)
{
    if(!(curAni_ = skeleton_->GetAniClip(aniName)))
        return false;
    aniLoop_ = loop;
    aniTime_ = 0.0f;
    curAniName_ = aniName;
    return true;
}

void VoxelModelAnimationDisplay::RestartAnimation(void)
{
    aniTime_ = 0.0f;
}

void VoxelModelAnimationDisplay::Update(float dT)
{
    assert(curAni_ != nullptr);
    aniTime_ += dT;
    if(aniTime_ >= curAni_->End() && aniLoop_)
    {
        while(aniTime_ >= curAni_->End())
            aniTime_ -= (curAni_->Start() - curAni_->End());
    }
}

void VoxelModelAnimationDisplay::Render(const Matrix &WVP)
{
    ID3D11Device *dev = Window::GetInstance().GetD3DDevice();
    ID3D11DeviceContext *DC = Window::GetInstance().GetD3DDeviceContext();

    std::vector<Matrix> boneMats;
    if(!skeleton_->GetTransMatrix(curAniName_, aniTime_, boneMats))
        return;

    shader_.Bind(DC);
    DC->IASetInputLayout(inputLayout_);

    //TODO

    DC->IASetInputLayout(nullptr);
    shader_.Unbind(DC);
}
