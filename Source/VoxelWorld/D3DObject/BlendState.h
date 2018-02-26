/*================================================================
Filename: BlendState.h
Date: 2018.1.27
Created by AirGuanZ
================================================================*/
#pragma once

#include <Utility/HelperFunctions.h>
#include <Utility/D3D11Header.h>
#include "Common.h"

class BlendState
{
public:
    BlendState(D3DObj_Noinit)
        : state_(nullptr)
    {

    }

    BlendState(void)
        : BlendState(D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP_ADD,
                     D3D11_BLEND_ONE, D3D11_BLEND_ZERO, D3D11_BLEND_OP_ADD)
    {
    
    }
    
    BlendState(D3D11_BLEND src, D3D11_BLEND dst, D3D11_BLEND_OP op,
               D3D11_BLEND srcA, D3D11_BLEND dstA, D3D11_BLEND_OP opA);
    
    BlendState(const BlendState &other)
    {
        state_ = other.state_;
        Helper::AddRefForCOMObjects(state_);
    }

    BlendState &operator=(const BlendState &other)
    {
        Helper::ReleaseCOMObjects(state_);
        state_ = other.state_;
        Helper::AddRefForCOMObjects(state_);
        return *this;
    }

    ~BlendState(void)
    {
        Helper::ReleaseCOMObjects(state_);
    }

    operator ID3D11BlendState* (void)
    {
        return state_;
    }

    ID3D11BlendState *GetState(void)
    {
        return state_;
    }

private:
    ID3D11BlendState *state_;
};
