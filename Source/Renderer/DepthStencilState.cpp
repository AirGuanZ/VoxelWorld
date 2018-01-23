/*================================================================
Filename: DepthStencilState.cpp
Date: 2018.1.23
Created by AirGuanZ
================================================================*/
#include "../Utility/HelperFunctions.h"
#include "../Window/Window.h"
#include "DepthStencilState.h"

DepthStencilState::DepthStencilState(bool depthTest, bool depthWrite)
    : state_(nullptr)
{
    D3D11_DEPTH_STENCIL_DESC dc;
    dc.DepthEnable = depthTest ? TRUE : FALSE;
    dc.DepthWriteMask = depthWrite ? D3D11_DEPTH_WRITE_MASK_ALL :
                                     D3D11_DEPTH_WRITE_MASK_ZERO;
    dc.DepthFunc = D3D11_COMPARISON_LESS;
    dc.StencilEnable = FALSE;
    dc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    dc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
    dc.FrontFace.StencilFunc = dc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    dc.FrontFace.StencilDepthFailOp = dc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dc.FrontFace.StencilPassOp = dc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dc.FrontFace.StencilFailOp = dc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

    Window::GetInstance().GetD3DDevice()->CreateDepthStencilState(&dc, &state_);
}

DepthStencilState::DepthStencilState(const DepthStencilState &other)
{
    state_ = other.state_;
    Helper::AddRefForCOMObjects(state_);
}

DepthStencilState &DepthStencilState::operator=(const DepthStencilState &other)
{
    state_ = other.state_;
    Helper::AddRefForCOMObjects(state_);
    return *this;
}

DepthStencilState::~DepthStencilState(void)
{
    Helper::ReleaseCOMObjects(state_);
}

DepthStencilState::operator ID3D11DepthStencilState* (void)
{
    return state_;
}

ID3D11DepthStencilState *DepthStencilState::GetState(void)
{
    return state_;
}
