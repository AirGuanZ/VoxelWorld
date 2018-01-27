/*================================================================
Filename: DepthStencilState.h
Date: 2018.1.23
Created by AirGuanZ
================================================================*/
#ifndef VW_DEPTH_STENCIL_STATE_H
#define VW_DEPTH_STENCIL_STATE_H

#include "../Utility/HelperFunctions.h"
#include "../Utility/D3D11Header.h"

class DepthStencilState
{
public:
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

#endif //VW_DEPTH_STENCIL_STATE_H
