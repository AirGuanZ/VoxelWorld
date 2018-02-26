/*================================================================
Filename: LiquidRenderer.h
Date: 2018.1.27
Created by AirGuanZ
================================================================*/
#pragma once

#include <memory>
#include <string>

#include <OWEShader.hpp>

#include <Utility/D3D11Header.h>
#include <Utility/Math.h>
#include <Utility/Uncopiable.h>

#include <D3DObject/BlendState.h>
#include <D3DObject/DepthStencilState.h>
#include <D3DObject/RasterState.h>
#include "BasicRenderer.h"

constexpr int LIQUID_RENDERER_TEXTURE_NUM = 1;
constexpr int LIQUID_RENDERER_TEXTURE_BLOCK_SIZE = 16;

class LiquidRenderer
{
public:
    using ShaderType = OWE::Shader<SS_VS, SS_PS>;
    using Uniforms = OWE::ShaderUniforms<SS_VS, SS_PS>;

    using Vertex = typename BasicRenderer::Vertex;

    LiquidRenderer(void);
    ~LiquidRenderer(void);

    bool Initialize(std::string &errMsg);
    void Destroy(void);
    bool IsAvailable(void) const;

    void Begin(void);
    void End(void);

    ShaderType &GetShader(void);

private:
    BasicRenderer basic_;

    std::unique_ptr<BlendState> blend_;
    std::unique_ptr<RasterState> raster_;
    std::unique_ptr<DepthStencilState> depth_;
};
