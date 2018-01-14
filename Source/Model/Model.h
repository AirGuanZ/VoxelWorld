/*================================================================
Filename: Model.h
Date: 2018.1.13
Created by AirGuanZ
================================================================*/
#ifndef VW_MODEL_H
#define VW_MODEL_H

#include <vector>

#include "../Utility/D3D11Header.h"

class Model
{
public:
    struct VertexBufferBinding
    {
        int startSlot = -1;
        std::vector<ID3D11Buffer*> bufs;
        std::vector<UINT> strides;
        std::vector<UINT> offsets;
    };

    bool IsAvailable(void) const;

    void Bind(void) const;
    void Unbind(void) const;

private:
    static const std::vector<ID3D11Buffer*> emptyVtxBufs_;

    VertexBufferBinding vtxBufBinding_;
};

#endif //VW_MODEL_H
