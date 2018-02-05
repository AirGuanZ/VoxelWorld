/*================================================================
Filename: ActorModel.h
Date: 2018.2.5
Created by AirGuanZ
================================================================*/
#ifndef VW_ACTOR_MODEL_H
#define VW_ACTOR_MODEL_H

#include <OWEShader.hpp>

#include "../D3DObject/BasicBuffer.h"

enum class ActorModelState
{
    Standing,
    Running
};

class ActorModel
{
public:
    void SetState(ActorModelState state);
    void Update(float deltaT);

    void Render(void);

private:
    ActorModelState state_;
    float time_;

    using Shader = OWE::Shader<SS_VS, SS_PS>;
    using Uniforms = OWE::ShaderUniforms<SS_VS, SS_PS>;

    struct Component
    {
        BasicBuffer<D3D11_BIND_VERTEX_BUFFER, false> vtx_;
        BasicBuffer<D3D11_BIND_INDEX_BUFFER, false>  idx_;
        int idxCount_;
    };
};

#endif //VW_ACTOR_MODEL_H
