/*================================================================
Filename: BlendState.h
Date: 2018.1.27
Created by AirGuanZ
================================================================*/
#ifndef VW_BLEND_STATE_H
#define VW_BLEND_STATE_H

#include "../Utility/D3D11Header.h"

class BlendState
{
public:
    BlendState(void);
    BlendState(D3D11_BLEND src, D3D11_BLEND dst, D3D11_BLEND_OP op,
               D3D11_BLEND srcA, D3D11_BLEND dstA, D3D11_BLEND_OP opA);
    BlendState(const BlendState &other);
    BlendState &operator=(const BlendState &other);
    ~BlendState(void);

    operator ID3D11BlendState* (void);
    ID3D11BlendState *GetState(void);

private:
    ID3D11BlendState *state_;
};

#endif //VW_BLEND_STATE_H
