/*================================================================
Filename: CarveRenderer.h
Date: 2018.1.21
Created by AirGuanZ
================================================================*/
#ifndef VW_CARVE_RENDERER_H
#define VW_CARVE_RENDERER_H

#include <memory>
#include <string>

#include <OWEShader.hpp>

#include "../Utility/D3D11Header.h"
#include "../Utility/Math.h"
#include "../Utility/Uncopiable.h"
#include "RasterState.h"

class CarveRenderer : public Uncopiable
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

    CarveRenderer(void);
    ~CarveRenderer(void);

    bool Initialize(std::string &errMsg);
    void Destroy(void);
    bool IsAvailable(void) const;

    void Begin(void);
    void End(void);

    ShaderType &GetShader(void);

private:
    ID3D11InputLayout *inputLayout_;
    ShaderType shader_;

    std::unique_ptr<RasterState> raster_;
};

#endif //VW_CARVE_RENDERER_H
