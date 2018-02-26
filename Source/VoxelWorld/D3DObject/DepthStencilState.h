/*================================================================
Filename: DepthStencilState.h
Date: 2018.1.23
Created by AirGuanZ
================================================================*/
#pragma once

#include <Utility/HelperFunctions.h>
#include <Utility/D3D11Header.h>
#include "Common.h"

class DepthStencilState
{
public:
    DepthStencilState(D3DObj_Noinit)
        : state_(nullptr)
    {

    }

    DepthStencilState(bool depthTest, bool depthWrite);

    DepthStencilState(const DepthStencilState &other)
    {
        state_ = other.state_;
        Helper::AddRefForCOMObjects(state_);
    }

    DepthStencilState &operator=(const DepthStencilState &other)
    {
        Helper::ReleaseCOMObjects(state_);
        state_ = other.state_;
        Helper::AddRefForCOMObjects(state_);
        return *this;
    }
    
    ~DepthStencilState(void);

    operator ID3D11DepthStencilState* (void)
    {
        return state_;
    }

    ID3D11DepthStencilState *GetState(void)
    {
        return state_;
    }

private:
    ID3D11DepthStencilState *state_;
};
