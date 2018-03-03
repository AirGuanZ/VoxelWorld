/*================================================================
Filename: VoxelModelEditor.h
Date: 2018.3.3
Created by AirGuanZ
================================================================*/
#pragma once

#include <memory>

#include <OWEShader.hpp>

#include <SkeletonAnimation/SkeletonData.h>
#include <SkeletonAnimation/SkeletonDataLoader.h>

#include "VoxelModel.h"

class VoxelModelEditor
{
public:
    using Shader = OWE::Shader<SS_VS, SS_PS>;
    using Uniforms = OWE::ShaderUniforms<SS_VS, SS_PS>;

    VoxelModelEditor(void);

    //VoxelModelEditor不具有models和skeleton数据的所有权
    bool Initialize(const std::vector<VoxelModel*> &models,
                    const std::vector<std::string> modelBindings,
                    const Skeleton::Skeleton *skeleton);
    void SetModel(int idx, VoxelModel *model);

    bool SetCurrentAnimation(const std::string &aniName, bool loop);
    void PlayAnimation(void);
    void PauseAnimation(void);
    void StopAnimation(void);

    void Update(float dT);
    void Render(void);

private:
    const std::vector<VoxelModel*> models_;
    const Skeleton::Skeleton *skeleton_;

    std::string curAni_;
    float aniTime_;
    bool aniLoop_;

    Shader shader_;
    std::unique_ptr<Uniforms> uniforms_;
};
