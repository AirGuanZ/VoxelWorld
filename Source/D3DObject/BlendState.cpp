/*================================================================
Filename: BlendState.cpp
Date: 2018.1.27
Created by AirGuanZ
================================================================*/
#include "../Utility/HelperFunctions.h"
#include "../Window/Window.h"
#include "BlendState.h"

BlendState::BlendState(D3D11_BLEND src, D3D11_BLEND dst, D3D11_BLEND_OP op,
                       D3D11_BLEND srcA, D3D11_BLEND dstA, D3D11_BLEND_OP opA)
    : state_(nullptr)
{
    D3D11_BLEND_DESC dc;
    dc.AlphaToCoverageEnable = FALSE;
    dc.IndependentBlendEnable = FALSE;
    dc.RenderTarget[0].BlendEnable = TRUE;
    dc.RenderTarget[0].BlendOp = op;
    dc.RenderTarget[0].BlendOpAlpha = opA;
    dc.RenderTarget[0].DestBlend = dst;
    dc.RenderTarget[0].DestBlendAlpha = dstA;
    dc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    dc.RenderTarget[0].SrcBlend = src;
    dc.RenderTarget[0].SrcBlendAlpha = srcA;

    Window::GetInstance().GetD3DDevice()->CreateBlendState(&dc, &state_);
}
