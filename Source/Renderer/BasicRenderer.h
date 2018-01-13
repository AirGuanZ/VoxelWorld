/*================================================================
Filename: BasicRenderer.h
Date: 2018.1.13
Created by AirGuanZ
================================================================*/
#ifndef VW_BASIC_RENDERER_H
#define VW_BASIC_RENDERER_H

#include <string>

#include <OWEShader.hpp>

#include "../Utility/D3D11Header.h"
#include "../Utility/Uncopiable.h"

class BasicRenderer : public Uncopiable
{
public:
    using ShaderType = OWE::Shader<SS_VS, SS_PS>;

    struct Vertex
    {
        DirectX::SimpleMath::Vector3 pos;        // POSITION
        DirectX::SimpleMath::Vector2 texCoord;   // TEXCOORD
        DirectX::SimpleMath::Vector3 normal;     // NORMAL
        DirectX::SimpleMath::Vector3 lightColor; // LIGHTCOLOR
    };

    BasicRenderer(void);
    ~BasicRenderer(void);

    bool Initialize(std::string &errMsg);
    void Destroy(void);
    bool IsAvailable(void) const;

    void Begin(void);
    void End(void);

private:
    ID3D11InputLayout *inputLayout_;
    ShaderType shader_;
};

#endif //VW_BASIC_RENDERER_H
