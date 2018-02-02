/*================================================================
Filename: ActorRenderer.h
Date: 2018.2.2
Created by AirGuanZ
================================================================*/
#ifndef VW_ACTOR_RENDERER_H
#define VW_ACTOR_RENDERER_H

#include <OWEShader.hpp>

#include "../Utility/D3D11Header.h"
#include "../Utility/Math.h"

class ActorRenderer
{
public:
    using ShaderType = OWE::Shader<SS_VS, SS_PS>;
    using Uniforms = OWE::ShaderUniforms<SS_VS, SS_PS>;

    struct Vertex
    {
        Vector3 pos;   //POSITION
        Vector3 nor;   //NORMAL
        Vector3 color; //COLOR
    };

    ActorRenderer(void);
    ~ActorRenderer(void);

    bool Initialize(void);
    void Destroy(void);
    bool IsAvailable(void) const;

    void Begin(void);
    void End(void);

    ShaderType &GetShader(void);

private:
    OWE::Shader<SS_VS, SS_PS> shader_;

    ID3D11InputLayout *inputLayout_;
};

#endif //VW_ACTOR_RENDERER_H
