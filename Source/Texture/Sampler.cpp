/*================================================================
Filename: Sampler.cpp
Date: 2018.1.21
Created by AirGuanZ
================================================================*/
#include "../Utility/HelperFunctions.h"
#include "Sampler.h"

Sampler::Sampler(void)
    : Sampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR,
              D3D11_TEXTURE_ADDRESS_CLAMP)
{
    
}

Sampler::Sampler(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode)
{
    sam_ = Helper::CreateSamplerState(filter, addressMode);
}

Sampler::Sampler(const Sampler &other)
    : sam_(other.sam_)
{
    Helper::AddRefForCOMObjects(sam_);
}

Sampler &Sampler::operator=(const Sampler &other)
{
    Helper::ReleaseCOMObjects(sam_);
    sam_ = other.sam_;
    Helper::AddRefForCOMObjects(sam_);
    return *this;
}

Sampler::~Sampler(void)
{
    Helper::ReleaseCOMObjects(sam_);
}

Sampler::operator ID3D11SamplerState* (void)
{
    return sam_;
}

ID3D11SamplerState *Sampler::GetSamplerState(void)
{
    return sam_;
}
