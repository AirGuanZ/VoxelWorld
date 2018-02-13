/*================================================================
Filename: RasterState.cpp
Date: 2018.1.21
Created by AirGuanZ
================================================================*/
#include <Utility/HelperFunctions.h>

#include "RasterState.h"

RasterState::RasterState(void)
    : RasterState(D3D11_FILL_SOLID, D3D11_CULL_BACK)
{

}

RasterState::RasterState(D3D11_FILL_MODE fillMode, D3D11_CULL_MODE cullMode)
    : state_(nullptr)
{
    D3D11_RASTERIZER_DESC dc;
    dc.FillMode              = fillMode;
    dc.CullMode              = cullMode;
    dc.FrontCounterClockwise = FALSE;
    dc.DepthBias             = 0;
    dc.SlopeScaledDepthBias  = 0.0f;
    dc.DepthBiasClamp        = 0.0f;
    dc.DepthClipEnable       = TRUE;
    dc.ScissorEnable         = FALSE;
    dc.MultisampleEnable     = FALSE;
    dc.AntialiasedLineEnable = FALSE;
    Window::GetInstance().GetD3DDevice()->CreateRasterizerState(
        &dc, &state_);
}

RasterState::~RasterState(void)
{
    Helper::ReleaseCOMObjects(state_);
}
