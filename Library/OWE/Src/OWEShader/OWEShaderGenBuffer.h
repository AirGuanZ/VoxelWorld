/*================================================================
Filename: OWEShaderGenBuffer.h
Date: 2017.11.23
Created by AirGuanZ
================================================================*/
#ifndef OWESHADER_GEN_BUFFER_H
#define OWESHADER_GEN_BUFFER_H

#include <cassert>
#include <d3d11.h>

namespace OWEShaderAux
{
    inline ID3D11Buffer *GenConstantBuffer(ID3D11Device *dev, size_t byteSize,
                                            bool dynamic, const D3D11_SUBRESOURCE_DATA *data)
    {
        assert(dev != nullptr && byteSize > 0);
        assert(dynamic == true || data != nullptr);

        ID3D11Buffer *buf = nullptr;
        D3D11_BUFFER_DESC bufDesc;
        bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bufDesc.ByteWidth = byteSize;
        bufDesc.MiscFlags = 0;
        bufDesc.StructureByteStride = 0;
        if(dynamic)
        {
            bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            bufDesc.Usage = D3D11_USAGE_DYNAMIC;
        }
        else
        {
            bufDesc.CPUAccessFlags = 0;
            bufDesc.Usage = D3D11_USAGE_IMMUTABLE;
        }

        if(FAILED(dev->CreateBuffer(&bufDesc, data, &buf)))
            return nullptr;
        return buf;
    }
}

#endif //OWESHADER_GEN_BUFFER_H
