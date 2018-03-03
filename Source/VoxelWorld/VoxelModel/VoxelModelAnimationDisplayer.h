/*================================================================
Filename: VoxelModelAnimationDisplayer.h
Date: 2018.3.3
Created by AirGuanZ
================================================================*/
#pragma once

#include <map>
#include <memory>

#include <OWEShader.hpp>

#include <SkeletonAnimation/SkeletonData.h>
#include <SkeletonAnimation/SkeletonDataLoader.h>

#include <D3DObject/InputLayout.h>
#include "VoxelModel.h"

class VoxelModelAnimationDisplayer
{
public:
    using Shader = OWE::Shader<SS_VS, SS_PS>;
    using Uniforms = OWE::ShaderUniforms<SS_VS, SS_PS>;

    VoxelModelAnimationDisplayer(void);

    //VoxelModelEditor不具有数据所有权
    bool Initialize(const std::vector<VoxelModel*> &models,
                    const Skeleton::Skeleton *skeleton,
                    const std::vector<std::string> &modelBindings,
                    const std::map<std::string, int> &boneMap,
                    std::string &errMsg);
    void Clear(void);
    void SetModel(int idx, VoxelModel *model);

    bool SetCurrentAnimation(const std::string &aniName, bool loop);
    void RestartAnimation(void);

    void Update(float dT);
    void Render(const Matrix &WVP);

private:
    struct VSCBTrans
    {
        Matrix WVP;
    };

private:
    std::vector<VoxelModel*> models_;
    std::vector<int> modelToBone_;
    const Skeleton::Skeleton *skeleton_;

    std::string curAniName_;
    const Skeleton::AniClip *curAni_;
    float aniTime_;
    bool aniLoop_;

    Shader shader_;
    std::unique_ptr<Uniforms> uniforms_;
    InputLayout inputLayout_;
};
