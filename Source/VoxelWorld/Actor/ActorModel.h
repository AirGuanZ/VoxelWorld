/*================================================================
Filename: ActorModel.h
Date: 2018.2.9
Created by AirGuanZ
================================================================*/
#ifndef VW_ACTOR_MODEL_H
#define VW_ACTOR_MODEL_H

#include <cstdint>
#include <memory>
#include <string>

#include <OWEShader.hpp>

#include <SkeletonAnimation\SkeletonDataLoader.h>

#include <Utility/Math.h>

#include "../D3DObject/BasicBuffer.h"
#include "../D3DObject/InputLayout.h"
#include "../D3DObject/Sampler.h"
#include "../Texture/Texture2D.h"
#include "Camera.h"

struct ActorModelVertex
{
    Vector3 pos;
    Vector3 nor;
    Vector2 uv;
};

struct ActorModelComponent
{
    int boneIndex;
    int idxCount;
    BasicBuffer<D3D11_BIND_VERTEX_BUFFER, false> vtxBuf;
    BasicBuffer<D3D11_BIND_INDEX_BUFFER, false>  idxBuf;
};

class ActorModel
{
public:
    using Shader = OWE::Shader<SS_VS, SS_PS>;
    using Uniforms = OWE::ShaderUniforms<SS_VS, SS_PS>;

    bool Initialize(std::string &errMsg);
    void Destroy(void);

    void SetTransform(const Matrix &mat);

    void SetAnimationClip(const std::string &clipName, bool loop);
    void Update(float deltaT);

    void Render(const Camera &cam);

private:
    Texture2D tex_;
    Sampler sampler_;

    Shader shader_;
    std::unique_ptr<Uniforms> uniforms_;
    InputLayout inputLayout_;

    Matrix worldTrans_;

    std::vector<ActorModelComponent> meshes_;

    Skeleton::Skeleton skeleton_;   //骨骼数据
    std::string currentAniClip_;    //当前动作
    bool aniClipLoop_;              //是否循环
    float t_;                       //播放时间
};

#endif //VW_ACTOR_MODEL_H
