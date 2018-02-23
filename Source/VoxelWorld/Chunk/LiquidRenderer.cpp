/*================================================================
Filename: TransparentRenderer.cpp
Date: 2018.1.27
Created by AirGuanZ
================================================================*/
#include <Utility/HelperFunctions.h>

#include "../Resource/ResourceName.h"
#include "LiquidRenderer.h"

LiquidRenderer::LiquidRenderer(void)
{

}

LiquidRenderer::~LiquidRenderer(void)
{
    Destroy();
}

bool LiquidRenderer::Initialize(std::string &errMsg)
{
    if(!basic_.Initialize(errMsg))
        return false;

    raster_ = std::make_unique<RasterState>(D3D11_FILL_SOLID, D3D11_CULL_NONE);
    blend_  = std::make_unique<BlendState>();
    depth_ = std::make_unique<DepthStencilState>(true, false);

    if(!raster_ || !blend_ || !depth_)
    {
        Destroy();
        return false;
    }

    return true;
}

void LiquidRenderer::Destroy(void)
{
    basic_.Destroy();
    blend_.reset();
    raster_.reset();
    depth_.reset();
}

bool LiquidRenderer::IsAvailable(void) const
{
    return basic_.IsAvailable();
}

void LiquidRenderer::Begin(void)
{
    ID3D11DeviceContext *DC = Window::GetInstance().GetD3DDeviceContext();

    basic_.Begin();
    DC->OMSetBlendState(*blend_, nullptr, 0xFFFFFFFF);
    DC->RSSetState(*raster_);
    DC->OMSetDepthStencilState(*depth_, 0);
}

void LiquidRenderer::End(void)
{
    ID3D11DeviceContext *DC = Window::GetInstance().GetD3DDeviceContext();

    DC->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
    DC->RSSetState(nullptr);
    DC->OMSetDepthStencilState(nullptr, 0);
    basic_.End();
}

LiquidRenderer::ShaderType &LiquidRenderer::GetShader(void)
{
    return basic_.GetShader();
}
