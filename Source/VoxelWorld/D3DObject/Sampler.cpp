/*================================================================
Filename: Sampler.cpp
Date: 2018.1.21
Created by AirGuanZ
================================================================*/
#include <Utility/HelperFunctions.h>

#include <Window/Window.h>
#include "Sampler.h"

Sampler::Sampler(void)
    : Sampler(D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR,
              D3D11_TEXTURE_ADDRESS_CLAMP)
{
    
}

namespace
{
    ID3D11SamplerState *CreateSamplerState(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addrMode)
    {
        D3D11_SAMPLER_DESC dc;
        dc.Filter = filter;
        dc.AddressU = addrMode;
        dc.AddressV = addrMode;
        dc.AddressW = addrMode;
        dc.MinLOD = -FLT_MAX;
        dc.MaxLOD = +FLT_MAX;
        dc.MipLODBias = 0.0f;
        dc.MaxAnisotropy = 1;
        dc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        dc.BorderColor[0] = dc.BorderColor[1] = dc.BorderColor[2] = dc.BorderColor[3] = 1.0f;

        ID3D11SamplerState *rt = nullptr;
        HRESULT hr = Window::GetInstance().GetD3DDevice()->CreateSamplerState(&dc, &rt);

        return SUCCEEDED(hr) ? rt : nullptr;
    }
}

Sampler::Sampler(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode)
{
    sam_ = CreateSamplerState(filter, addressMode);
}

Sampler::~Sampler(void)
{
    Helper::ReleaseCOMObjects(sam_);
}
