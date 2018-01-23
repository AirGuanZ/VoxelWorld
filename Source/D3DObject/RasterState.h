/*================================================================
Filename: RasterState.h
Date: 2018.1.21
Created by AirGuanZ
================================================================*/
#ifndef VW_RASTER_STATE_H
#define VW_RASTER_STATE_H

#include "../Window/Window.h"

class RasterState
{
public:
    RasterState(void);
    RasterState(D3D11_FILL_MODE fillMode, D3D11_CULL_MODE cullMode);
    RasterState(const RasterState &other);
    RasterState &operator=(const RasterState &other);
    ~RasterState(void);

    operator ID3D11RasterizerState* (void);
    ID3D11RasterizerState *GetState(void);

private:
    ID3D11RasterizerState *state_;
};

#endif //VW_RASTER_STATE_H
