/*================================================================
Filename: BasicRenderer.h
Date: 2018.1.13
Created by AirGuanZ
================================================================*/
#ifndef VW_BASIC_RENDERER_H
#define VW_BASIC_RENDERER_H

#include <memory>
#include <string>

#include <OWEShader.hpp>

#include "../Texture/Texture2D.h"
#include "../Utility/D3D11Header.h"
#include "../Utility/Math.h"
#include "../Utility/Uncopiable.h"

constexpr int BASIC_RENDERER_TEXTURE_NUM = 1;
constexpr int BASIC_RENDERER_TEXTURE_BLOCK_SIZE = 16;

class BasicRenderer : public Uncopiable
{
public:
    using ShaderType = OWE::Shader<SS_VS, SS_PS>;
    using Uniforms = OWE::ShaderUniforms<SS_VS, SS_PS>;

    struct Vertex
    {
        Vector3 pos;        // POSITION
        Vector2 texCoord;   // TEXCOORD
        Vector3 lightColor; // LIGHTCOLOR
        float   sunlight;   // SUNLIGHT
    };

    BasicRenderer(void);
    ~BasicRenderer(void);

    bool Initialize(std::string &errMsg);
    void Destroy(void);
    bool IsAvailable(void) const;

    void Begin(void);
    void End(void);

    ShaderType &GetShader(void);

    void SetViewProjMatrix(const Matrix &mat);
    void SetSunlightFactor(float sunlight);

    void SetTexture(Texture2D tex);
    void SetSampler(ID3D11SamplerState *sampler);

private:
    struct VSCBTrans
    {
        Matrix WVP;
    };

    struct PSCBLight
    {
        float sunlight;
        float pad0, pad1, pad2;
    };

    std::unique_ptr<Uniforms> uniforms_;

    ID3D11InputLayout *inputLayout_;
    ShaderType shader_;
};

#endif //VW_BASIC_RENDERER_H
