/*================================================================
Filename: Sampler.h
Date: 2018.1.21
Created by AirGuanZ
================================================================*/
#pragma once

#include <Utility\HelperFunctions.h>

#include <Window\Window.h>
#include "Common.h"

class Sampler
{
public:
    Sampler(D3DObj_Noinit)
        : sam_(nullptr)
    {

    }

    Sampler(void);
    Sampler(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode);
    
    Sampler(const Sampler &other)
        : sam_(other.sam_)
    {
        Helper::AddRefForCOMObjects(sam_);
    }

    Sampler &operator=(const Sampler &other)
    {
        Helper::ReleaseCOMObjects(sam_);
        sam_ = other.sam_;
        Helper::AddRefForCOMObjects(sam_);
        return *this;
    }
    
    ~Sampler(void);
    
    operator ID3D11SamplerState* (void)
    {
        return sam_;
    }
    
    ID3D11SamplerState *GetSamplerState(void)
    {
        return sam_;
    }

private:
    ID3D11SamplerState *sam_;
};
