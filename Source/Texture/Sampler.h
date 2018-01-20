/*================================================================
Filename: Sampler.h
Date: 2018.1.21
Created by AirGuanZ
================================================================*/
#ifndef VW_SAMPLER_H
#define VW_SAMPLER_H

#include "../Window/Window.h"

class Sampler
{
public:
    Sampler(void);
    Sampler(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode);
    Sampler(const Sampler &other);
    Sampler &operator=(const Sampler &other);
    ~Sampler(void);
    
    operator ID3D11SamplerState* (void);
    
    ID3D11SamplerState *GetSamplerState(void);

private:
    ID3D11SamplerState *sam_;
};

#endif //VW_SAMPLER_H
