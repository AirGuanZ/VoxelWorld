/*================================================================
Filename: DepthStencilState.h
Date: 2018.1.23
Created by AirGuanZ
================================================================*/
#ifndef VW_DEPTH_STENCIL_STATE_H
#define VW_DEPTH_STENCIL_STATE_H

#include "../Utility/D3D11Header.h"

class DepthStencilState
{
public:
    DepthStencilState(bool depthTest, bool depthWrite);
    DepthStencilState(const DepthStencilState &other);
    DepthStencilState &operator=(const DepthStencilState &other);
    ~DepthStencilState(void);

    operator ID3D11DepthStencilState* (void);
    ID3D11DepthStencilState *GetState(void);

private:
    ID3D11DepthStencilState *state_;
};

#endif //VW_DEPTH_STENCIL_STATE_H
