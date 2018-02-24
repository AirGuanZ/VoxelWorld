/*================================================================
Filename: RasterState.h
Date: 2018.1.21
Created by AirGuanZ
================================================================*/
#ifndef VW_RASTER_STATE_H
#define VW_RASTER_STATE_H

#include <Utility/HelperFunctions.h>

#include <Window/Window.h>

class RasterState
{
public:
    RasterState(void);
    RasterState(D3D11_FILL_MODE fillMode, D3D11_CULL_MODE cullMode);
    
    RasterState(const RasterState &other)
    {
        state_ = other.state_;
        Helper::AddRefForCOMObjects(state_);
    }

    RasterState &operator=(const RasterState &other)
    {
        Helper::ReleaseCOMObjects(state_);
        state_ = other.state_;
        Helper::AddRefForCOMObjects(state_);
        return *this;
    }
    
    ~RasterState(void);

    operator ID3D11RasterizerState* (void)
    {
        return state_;
    }

    ID3D11RasterizerState *GetState(void)
    {
        return state_;
    }

private:
    ID3D11RasterizerState *state_;
};

#endif //VW_RASTER_STATE_H
