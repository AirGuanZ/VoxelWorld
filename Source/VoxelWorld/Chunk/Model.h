/*================================================================
Filename: Model.h
Date: 2018.1.13
Created by AirGuanZ
================================================================*/
#pragma once

#include <vector>

#include <Utility/D3D11Header.h>

class Model
{
public:
    struct VertexBufferBinding
    {
        int startSlot = -1;
        int idxCount  = -1;
        DXGI_FORMAT indicesFormat = DXGI_FORMAT_UNKNOWN;
        ID3D11Buffer *indices = nullptr;
        std::vector<ID3D11Buffer*> bufs;
        std::vector<UINT> strides;
        std::vector<UINT> offsets;
    };

    Model(void) = default;

    bool IsAvailable(void) const;

    void Draw(void) const;

private:
    static const std::vector<ID3D11Buffer*> emptyVtxBufs_;

protected:
    VertexBufferBinding vtxBufBinding_;
};
