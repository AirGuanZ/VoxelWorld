/*================================================================
Filename: BlendState.h
Date: 2018.1.27
Created by AirGuanZ
================================================================*/
#ifndef VW_BLEND_STATE_H
#define VW_BLEND_STATE_H

#include <Utility/HelperFunctions.h>
#include <Utility/D3D11Header.h>

class BlendState
{
public:
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

#endif //VW_BLEND_STATE_H
