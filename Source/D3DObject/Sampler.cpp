/*================================================================
Filename: Sampler.cpp
Date: 2018.1.21
Created by AirGuanZ
================================================================*/
#include "../Utility/HelperFunctions.h"
#include "Sampler.h"

Sampler::Sampler(void)
    : Sampler(D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR,
              D3D11_TEXTURE_ADDRESS_CLAMP)
{
    
}

Sampler::Sampler(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode)
{
    sam_ = Helper::CreateSamplerState(filter, addressMode);
}

Sampler::~Sampler(void)
{
    Helper::ReleaseCOMObjects(sam_);
}
